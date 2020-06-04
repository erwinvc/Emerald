#include "stdafx.h"

void AssetWatcher::Watch() {
	Sleep(500);

	for (auto it = m_paths.begin(); it != m_paths.end();) {
		if (!filesystem::exists(it->first)) {
			HandleChange(it->second, WatchType::REMOVED);
			it = m_paths.erase(it);
		} else it++;
	}

	for (String& dir : m_dirs) {
		for (auto& file : std::filesystem::recursive_directory_iterator(dir)) {
			if (!filesystem::exists(file)) continue;
			auto current_file_last_write_time = std::filesystem::last_write_time(file);

			filesystem::path path = file.path();
			auto it = m_paths.find(path.string());
			if (it == m_paths.end()) {
				Tracker newTracker(Path(path.string()), current_file_last_write_time);
				m_paths[file.path().string()] = newTracker;
				HandleChange(newTracker, WatchType::ADDED);
			} else {
				if (it->second.m_time != current_file_last_write_time) {
					it->second.m_time = current_file_last_write_time;
					HandleChange(it->second, WatchType::CHANGED);
				}
			}
		}
	}
}

void AssetWatcher::HandleChange(const Tracker& tracker, WatchType type) {
	map<String, function<void(const Tracker&)>>* handlers = nullptr;
	switch (type) {
		case WatchType::ADDED: handlers = &m_addedHandlers; break;
		case WatchType::REMOVED: handlers = &m_removedHandlers; break;
		case WatchType::CHANGED: handlers = &m_changedHandlers; break;
	}

	//switch (type) {
	//	case WatchType::ADDED:   LOG("[~yAssets~x] Detected added ~1%s", tracker.m_path.GetFileName().c_str()); break;
	//	case WatchType::REMOVED: LOG("[~yAssets~x] Detected removed ~1%s", tracker.m_path.GetFileName().c_str()); break;
	//	case WatchType::CHANGED: LOG("[~yAssets~x] Detected change in ~1%s", tracker.m_path.GetFileName().c_str()); break;
	//}

	auto it = handlers->find(tracker.m_path.GetExtention());
	if (it != handlers->end()) {
		m_queue.Add({ it->second, tracker });
	}
}

void AssetWatcher::Initialize() {
	if (m_initialized) return;
	m_thread = GetThreadManager()->RegisterThread("AssetWatcher", [] {GetAssetWatcher()->Watch(); });
	LOG("[~yAssets~x] Initialized Asset Watcher");
	AddDirectory("res/shaders", false);
	AddChangeHandler(WatchType::CHANGED, "vert", [=](const Tracker& file) { GetShaderManager()->ReloadShaderByFileName(file.m_path.GetFullPath()); });
	AddChangeHandler(WatchType::CHANGED, "frag", [=](const Tracker& file) { GetShaderManager()->ReloadShaderByFileName(file.m_path.GetFullPath()); });
	AddChangeHandler(WatchType::CHANGED, "tesc", [=](const Tracker& file) { GetShaderManager()->ReloadShaderByFileName(file.m_path.GetFullPath()); });
	AddChangeHandler(WatchType::CHANGED, "tese", [=](const Tracker& file) { GetShaderManager()->ReloadShaderByFileName(file.m_path.GetFullPath()); });
	m_initialized = true;
}

void AssetWatcher::AddDirectory(const String& dir, bool watchSubTree) {
	if (FileSystem::DoesFileExist(dir)) {
		if (!Utils::VectorContains(m_dirs, dir)) {
			for (auto& file : filesystem::recursive_directory_iterator(dir)) {
				m_paths[file.path().string()] = Tracker(Path(file.path().string().c_str()), filesystem::last_write_time(file));
			}
			m_dirs.push_back(dir);
			LOG("[~yAssets~x] Added ~1%s~x to Asset Watcher", dir.c_str());
			return;
		}
	} else LOG("[~yAssets~x] Failed to create Asset Watcher for ~x%s", dir.c_str());
}

void AssetWatcher::AddChangeHandler(WatchType type, const String& extention, function<void(const Tracker&)> handler) {
	map<String, function<void(const Tracker&)>>* handlers = nullptr;
	switch (type) {
		case WatchType::ADDED: handlers = &m_addedHandlers; break;
		case WatchType::REMOVED: handlers = &m_removedHandlers; break;
		case WatchType::CHANGED: handlers = &m_changedHandlers; break;
	}
	if (handlers->find(extention) == handlers->end()) {
		handlers->emplace(extention, handler);
	} else LOG_ERROR("[~yAssets~x] Asset Watcher already contains handler for ~1%s", extention.c_str());
}

void AssetWatcher::HandleQueue() {
	QueueEntry entry;
	while (m_queue.Size() > 0) {
		if (m_queue.TryToGet(entry)) {
			entry.m_function(entry.m_file);
		}
	}
}