#pragma once
#include "editor/events/editorProjectOpenedEvent.h"

namespace emerald {
	class FileWatcher {
		STATIC_CLASS(FileWatcher)

	public:
		static void initialize();
		static void shutdown();
		static void processEvents();
		static void processNotification(const FILE_NOTIFY_INFORMATION* info, const std::filesystem::path& basePath);
	private:
		static void onEditorProjectOpenedEvent(const EditorProjectOpenedEvent& e);
		static void setDirectory(const std::filesystem::path dir);
		static void watchThread();
	};
}