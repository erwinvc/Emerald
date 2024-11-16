#pragma once

namespace emerald {
	class EngineLoading {
	public:
		static void startLoading(const std::string& message = "Loading, please wait...", bool canCancel = false);
		static void stopLoading();
		static void setProgress(float progress);
		static void setTaskDescription(const std::string& description);
		static float getProgress();
		static bool isLoading();
		static const std::string& getMessage();
		static const std::string& getTaskDescription();
		static void draw();

	private:
		static inline std::mutex m_mutex;
		static inline bool m_loading = false;
		static inline bool m_canCancel = false;
		static inline bool m_isCanceled = false;
		static inline float m_progress = 0.0f;
		static inline std::string m_message;
		static inline std::string m_taskDescription;
	};
}