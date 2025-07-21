#include "eepch.h"
#include "fileWatcher.h"
#include "engine/events/eventSystem.h"
#include "utils/threading/threadManager.h"
#include "engine/events/fileChangedEvent.h"
#include "utils/datastructures/buffer.h"
#include "core/application/application.h"

namespace emerald {
	static constexpr uint32_t BUFFER_SIZE = 32768; // 32KB

	// Dynamic directory
	static std::filesystem::path m_currentDir;
	static HANDLE m_directoryHandle{ INVALID_HANDLE_VALUE };
	static std::atomic<bool> m_running{ true };
	static UniqueRef<OVERLAPPED> m_overlapped;
	static HANDLE m_shutdownEvent;
	static Buffer<byte> m_buffer;
	static std::mutex m_eventQueueMutex;
	static AsyncQueue<FileChangedEvent> m_eventQueues[(uint32_t)FileChangedEventType::_COUNT]{};
	static std::condition_variable m_handleCondition;
	static std::mutex m_handleMutex;

	// Static directory for editor
	static std::filesystem::path m_staticDir;
	static HANDLE m_staticDirectoryHandle{ INVALID_HANDLE_VALUE };
	static UniqueRef<OVERLAPPED> m_staticOverlapped;
	static Buffer<byte> m_staticBuffer;

	void FileWatcher::initialize() {
		m_buffer.reserve(BUFFER_SIZE);
		m_buffer.zeroInitialize();

		m_overlapped = UniqueRef<OVERLAPPED>::create();
		m_overlapped->hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		m_shutdownEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

		m_staticDir = "I:/Development/C++/Emerald/editor/res/shaders";
		m_staticBuffer.reserve(BUFFER_SIZE);
		m_staticBuffer.zeroInitialize();
		m_staticOverlapped = UniqueRef<OVERLAPPED>::create();
		m_staticOverlapped->hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

		m_staticDirectoryHandle = CreateFileW(
			m_staticDir.wstring().c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			nullptr
		);
		if (m_staticDirectoryHandle == INVALID_HANDLE_VALUE) {
			Log::error("[FileWatcher] Failed to create handle for static dir: {}", m_staticDir.u8string());
		}

		ThreadManager::createAndRegisterThread(ThreadType::FILEWATCHER, ProfilerThreadType::IGNORED, ThreadPriority::NORMAL, "File watcher", watchThread);

		EventSystem::subscribe<EditorProjectOpenedEvent>(onEditorProjectOpenedEvent);
	}

	void FileWatcher::shutdown() {
		m_running = false;
		SetEvent(m_shutdownEvent);

		if (m_shutdownEvent != nullptr) {
			CloseHandle(m_shutdownEvent);
			m_shutdownEvent = nullptr;
		}

		if (m_staticDirectoryHandle != INVALID_HANDLE_VALUE) {
			CloseHandle(m_staticDirectoryHandle);
			m_staticDirectoryHandle = INVALID_HANDLE_VALUE;
		}

		if (m_directoryHandle != INVALID_HANDLE_VALUE) {
			CloseHandle(m_directoryHandle);
			m_directoryHandle = INVALID_HANDLE_VALUE;
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
			Log::error("[FileWatcher] Watch path does not exist: {}", m_currentDir.u8string());
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
			Log::error("[FileWatcher] Failed to create directory handle: {}", m_currentDir.u8string());
		}

		m_handleCondition.notify_all();
	}

	static bool issueRead(HANDLE directoryHandle, Buffer<byte>& buffer, UniqueRef<OVERLAPPED>& overlapped) {
		if (directoryHandle == INVALID_HANDLE_VALUE)
			return false;

		DWORD bytesReturned = 0;
		BOOL success = ReadDirectoryChangesW(
			directoryHandle,
			buffer.data(),
			static_cast<DWORD>(buffer.size()),
			TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_ATTRIBUTES |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_CREATION,
			&bytesReturned,
			overlapped.raw(),
			nullptr
		);

		if (!success && GetLastError() != ERROR_IO_PENDING) {
			Log::error("[FileWatcher] Failed to read directory changes (handle={}): err={}",
				(void*)directoryHandle, GetLastError());
			return false;
		}
		return true;
	}

	static void handleDirectoryChange(HANDLE directoryHandle, const std::filesystem::path& basePath, Buffer<byte>& buffer, UniqueRef<OVERLAPPED>& overlapped) {
		DWORD bytesReturned = 0;
		if (!GetOverlappedResult(directoryHandle, overlapped.raw(), &bytesReturned, FALSE)) {
			ResetEvent(overlapped->hEvent);
			issueRead(directoryHandle, buffer, overlapped);
			return;
		}
		if (bytesReturned == 0) {
			ResetEvent(overlapped->hEvent);
			issueRead(directoryHandle, buffer, overlapped);
			return;
		}
		auto* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer.data());
		while (info) {
			Log::info("Got change for basePath: {}", basePath.string());
			FileWatcher::processNotification(info, basePath);

			if (info->NextEntryOffset == 0) {
				break;
			}
			info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
				reinterpret_cast<uint8_t*>(info) + info->NextEntryOffset
				);
		}
		ResetEvent(overlapped->hEvent);
		issueRead(directoryHandle, buffer, overlapped);
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

			issueRead(m_directoryHandle, m_buffer, m_overlapped);
			issueRead(m_staticDirectoryHandle, m_staticBuffer, m_staticOverlapped);

			HANDLE handles[3] = {
			m_overlapped->hEvent,
			m_staticOverlapped->hEvent,
			m_shutdownEvent
			};

			DWORD waitResult = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
			if (waitResult == WAIT_OBJECT_0) {
				// Dynamic directory signaled
				handleDirectoryChange(
					m_directoryHandle,
					m_currentDir,
					m_buffer,
					m_overlapped
				);
			} else if (waitResult == WAIT_OBJECT_0 + 1) {
				// Static directory signaled
				handleDirectoryChange(
					m_staticDirectoryHandle,
					m_staticDir,
					m_staticBuffer,
					m_staticOverlapped
				);
			} else if (waitResult == WAIT_OBJECT_0 + 2) {
				// Shutdown event triggered
				return;
			}
		}
	}

	void FileWatcher::processNotification(const FILE_NOTIFY_INFORMATION* info, const std::filesystem::path& basePath) {
		std::wstring filename(info->FileName, info->FileNameLength / sizeof(WCHAR));
		std::filesystem::path fullPath = basePath / filename;

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

		std::lock_guard<std::mutex> queueLock(m_eventQueueMutex);
		m_eventQueues[(uint32_t)eventType].emplace(FileChangedEvent{ eventType,fullPath,now, basePath == m_staticDir });
	}

	void FileWatcher::processEvents() {
		if (!App->isFocused()) return;
		for (int i = 0; i < (uint32_t)FileChangedEventType::_COUNT; i++) {
			AsyncQueue<FileChangedEvent> events;
			m_eventQueues[i].swap(events);

			//events.removeDuplicates();
			FileChangedEvent event;
			while (!events.empty()) {
				events.tryGet(event);
				EventSystem::dispatch<FileChangedEvent>(event);
			}
		}
	}
}