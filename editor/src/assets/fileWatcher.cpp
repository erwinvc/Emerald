#include "eepch.h"
#include "fileWatcher.h"
#include "engine/events/eventSystem.h"
#include "utils/threading/threadManager.h"
#include "engine/events/fileChangedEvent.h"
#include "utils/datastructures/buffer.h"
#include "core/application/application.h"

namespace emerald {
	static constexpr uint32_t BUFFER_SIZE = 32768; // 32KB

	static std::filesystem::path m_currentDir;
	static HANDLE m_directoryHandle{ INVALID_HANDLE_VALUE };
	static std::atomic<bool> m_running{ true };
	static UniqueRef<OVERLAPPED> m_overlapped;
	static HANDLE m_shutdownEvent;
	static Buffer<byte> m_buffer;
	static std::mutex m_eventQueueMutex;
	static std::unordered_map<std::filesystem::path, std::chrono::system_clock::time_point> lastEventTimes_;
	static AsyncQueue<FileChangedEvent> m_eventQueues[(uint32_t)FileChangedEventType::_COUNT]{};
	static std::condition_variable m_handleCondition;
	static std::mutex m_handleMutex;

	void FileWatcher::initialize() {
		m_buffer.reserve(BUFFER_SIZE);
		m_buffer.zeroInitialize();

		m_overlapped = UniqueRef<OVERLAPPED>::create();
		m_overlapped->hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		m_shutdownEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

		ThreadManager::createAndRegisterThread(ThreadType::FILEWATCHER, ThreadPriority::NORMAL, "File watcher", watchThread);

		EventSystem::subscribe<EditorProjectOpenedEvent>(onEditorProjectOpenedEvent);
	}

	void FileWatcher::shutdown() {
		m_running = false;
		SetEvent(m_shutdownEvent);
		if (m_shutdownEvent != nullptr) {
			CloseHandle(m_shutdownEvent);
		}
		m_handleCondition.notify_all();
	}

	void FileWatcher::onEditorProjectOpenedEvent(const EditorProjectOpenedEvent& e) {
		if (e.isValid()) {
			setDirectory(e.getPath());
		} else {
			setDirectory(std::filesystem::path());
		}
	}

	void FileWatcher::setDirectory(const std::filesystem::path dir) {
		std::unique_lock<std::mutex> lock(m_handleMutex);

		m_currentDir = dir;

		if (!std::filesystem::exists(m_currentDir)) {
			Log::error("[FileWatcher] Watch path does not exist: {}", m_currentDir.string());
		}

		if (m_directoryHandle != INVALID_HANDLE_VALUE) {
			CloseHandle(m_directoryHandle);
		}

		m_directoryHandle = CreateFileW(
			m_currentDir.wstring().c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			nullptr
		);

		if (m_directoryHandle == INVALID_HANDLE_VALUE) {
			Log::error("[FileWatcher] Failed to create directory handle: {}", m_currentDir.string());
		}

		m_handleCondition.notify_all();
	}

	void FileWatcher::watchThread() {
		while (m_running) {
			{
				std::unique_lock<std::mutex> lock(m_handleMutex);
				m_handleCondition.wait(lock, [] { return m_directoryHandle != INVALID_HANDLE_VALUE || !m_running; });
				if (!m_running) {
					return;
				}
			}
			DWORD bytesReturned = 0;
			auto success = ReadDirectoryChangesW(
				m_directoryHandle,
				m_buffer.data(),
				static_cast<DWORD>(m_buffer.size()),
				true,
				FILE_NOTIFY_CHANGE_FILE_NAME |
				FILE_NOTIFY_CHANGE_DIR_NAME |
				FILE_NOTIFY_CHANGE_ATTRIBUTES |
				FILE_NOTIFY_CHANGE_SIZE |
				FILE_NOTIFY_CHANGE_LAST_WRITE |
				FILE_NOTIFY_CHANGE_CREATION,
				&bytesReturned,
				m_overlapped.raw(),
				nullptr
			);

			if (!success && GetLastError() != ERROR_IO_PENDING) {
				Log::error("[FileWatcher] Failed to read directory changes: {}", GetLastError());
				continue;
			}

			HANDLE handles[] = { m_overlapped->hEvent, m_shutdownEvent };
			DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE, INFINITE);

			if (waitResult == WAIT_OBJECT_0) {
				if (!GetOverlappedResult(m_directoryHandle, m_overlapped.raw(), &bytesReturned, FALSE)) {
					continue;
				}

				if (bytesReturned == 0) {
					continue;
				}

				auto* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(m_buffer.data());
				while (info) {
					processNotification(info);
					if (info->NextEntryOffset == 0) break;
					info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
						reinterpret_cast<uint8_t*>(info) + info->NextEntryOffset
						);
				}
			} else if (waitResult == WAIT_OBJECT_0 + 1) {
				// Shutdown event triggered
				return;
			}
		}
	}

	void FileWatcher::processNotification(const FILE_NOTIFY_INFORMATION* info) {
		std::wstring filename(info->FileName, info->FileNameLength / sizeof(WCHAR));
		std::filesystem::path fullPath = m_currentDir / filename;

		auto now = std::chrono::system_clock::now();

		FileChangedEventType eventType;
		switch (info->Action) {
			case FILE_ACTION_ADDED:
			case FILE_ACTION_RENAMED_NEW_NAME:
				eventType = FileChangedEventType::CREATED;
				break;
			case FILE_ACTION_REMOVED:
			case FILE_ACTION_RENAMED_OLD_NAME:
				eventType = FileChangedEventType::DELETED;
				break;
			case FILE_ACTION_MODIFIED:
				eventType = FileChangedEventType::MODIFIED;
				break;
			default:
				return;
		}

		std::lock_guard queueLock(m_eventQueueMutex);
		m_eventQueues[(uint32_t)eventType].emplace(FileChangedEvent{ eventType,fullPath,now, });
	}

	void FileWatcher::processEvents() {
		if (!App->isFocused()) return;
		for (int i = 0; i < (uint32_t)FileChangedEventType::_COUNT; i++) {
			AsyncQueue<FileChangedEvent> events;
			m_eventQueues[i].swap(events);

			FileChangedEvent event;
			while (!events.empty()) {
				events.tryGet(event);
				EventSystem::dispatch<FileChangedEvent>(event);
			}
		}
	}
}