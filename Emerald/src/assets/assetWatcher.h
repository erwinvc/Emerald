#pragma once
#include <filesystem>

class AssetWatcher : public Singleton<AssetWatcher> {
private:
	struct QueueEntry {
		function<void(const String&)> m_function;
		String m_file;
	};
	AsyncQueue<QueueEntry> m_queue;
	unordered_map<String, filesystem::file_time_type> m_paths;
	map<String, function<void(const String&)>> m_handlers;
	AssetRef<Thread> m_thread;
	bool m_initialized = false;
	vector<String> m_dirs;
	void Watch() {
		Sleep(500);

		for (auto it = m_paths.begin(); it != m_paths.end();) {
			if (!filesystem::exists(it->first)) {
				it = m_paths.erase(it);
			} else it++;
		}

		for (String& dir : m_dirs) {
			for (auto &file : std::filesystem::recursive_directory_iterator(dir)) {
				auto current_file_last_write_time = std::filesystem::last_write_time(file);

				String path = file.path().string();
				if (m_paths.find(path) == m_paths.end()) {
					m_paths[file.path().string()] = current_file_last_write_time;
				} else {
					if (m_paths[path] != current_file_last_write_time) {
						m_paths[path] = current_file_last_write_time;
						HandleChange(dir, path);
					}
				}
			}
		}
	}

	void HandleChange(const String& dir, const filesystem::path& path) {
		String fileName = path.filename().string();
		String extention = path.extension().string();
		auto it = m_handlers.find(extention);
		LOG("[~yAssets~x] Detected change in ~1%s", fileName.c_str());
		if (it != m_handlers.end()) {
			m_queue.Add({ it->second, dir + "/" + fileName.substr(0, fileName.size() - extention.size()) });
		}
	}
public:
	void Initialize() {
		if (m_initialized) return;
		m_thread = GetThreadManager()->RegisterThread("AssetWatcher", [] {GetInstance()->Watch(); });
		LOG("[~yAssets~x] Initialized Asset Watcher");
		AddDirectory("res/shader", false);
		AddChangeHandler(".vert", [](const String& file) { GetShaderManager()->ReloadShaderByFileName(file); });
		AddChangeHandler(".frag", [](const String& file) { GetShaderManager()->ReloadShaderByFileName(file); });
		AddChangeHandler(".tesc", [](const String& file) { GetShaderManager()->ReloadShaderByFileName(file); });
		AddChangeHandler(".tese", [](const String& file) { GetShaderManager()->ReloadShaderByFileName(file); });
		m_initialized = true;
	}

	void AddDirectory(const String& dir, bool watchSubTree) {
		if (!Utils::VectorContains(m_dirs, dir)) {
			for (auto &file : filesystem::recursive_directory_iterator(dir)) {
				m_paths[file.path().string()] = std::filesystem::last_write_time(file);
			}
			m_dirs.push_back(dir);
			LOG("[~yAssets~x] Added ~1%s~x to Asset Watcher", dir.c_str());
		} else LOG("[~yAssets~x] Failed to create Asset Watcher for ~x%s", dir.c_str());
	}

	void AddChangeHandler(const String& extention, function<void(const String&)> handler) {
		if (m_handlers.find(extention) == m_handlers.end()) {
			m_handlers.emplace(extention, handler);
		} else LOG_ERROR("[~yAssets~x] Asset Watcher already contains handler for ~1%s", extention.c_str());
	}

	void HandleQueue() {
		QueueEntry entry;
		if (m_queue.TryToGet(entry)) {
			entry.m_function(entry.m_file);
		}
	}
};

static AssetWatcher* GetAssetWatcher() {
	return AssetWatcher::GetInstance();
}