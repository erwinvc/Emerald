#pragma once
#include <filesystem>

enum class WatchType {
	ADDED,
	REMOVED,
	CHANGED
};

struct Tracker {
	Path m_path;
	filesystem::file_time_type m_time;
	Tracker(Path& path, filesystem::file_time_type& time) : m_path(path), m_time(time) {}
	Tracker() : m_path(), m_time(filesystem::file_time_type()) {}
};

class AssetWatcher : public Singleton<AssetWatcher> {
private:
	struct QueueEntry {
		function<void(const Tracker&)> m_function;
		Tracker m_file;
	};

	AsyncQueue<QueueEntry> m_queue;
	unordered_map<String, Tracker> m_paths;
	map<String, function<void(const Tracker&)>> m_removedHandlers;
	map<String, function<void(const Tracker&)>> m_addedHandlers;
	map<String, function<void(const Tracker&)>> m_changedHandlers;
	AssetRef<Thread> m_thread;
	bool m_initialized = false;
	vector<String> m_dirs;
	void Watch();
	void HandleChange(const Tracker& tracker, WatchType type);
	
public:
	void Initialize();
	void AddDirectory(const String& dir, bool watchSubTree);
	void AddChangeHandler(WatchType type, const String& extention, function<void(const Tracker&)> handler);
	void HandleQueue();
};

inline AssetWatcher* GetAssetWatcher() {
	return AssetWatcher::GetInstance();
}