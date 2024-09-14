#pragma once

namespace emerald {
	class LogPanel {
	public:
		LogPanel();
		void draw();

	private:
		bool m_collapseLogs;
		bool m_autoscroll;
		LogMessage m_selectedMessage;
		char m_searchString[256];
		float m_logPanelHeightRatio;
		uint32_t m_selectedMessageIndex;
		uint32_t m_selectedLogLevel;
		uint32_t m_lastLogCount;

		std::unordered_map<size_t, int> m_logMessageCounts;
		std::vector<const LogMessage*> m_collapsedLogs;

		void drawButtons();
		void drawSplitter(float totalHeight, float logPanelHeight);
		void drawDetailsPanel(float panelHeight);
		void forceRefresh();
	};
}