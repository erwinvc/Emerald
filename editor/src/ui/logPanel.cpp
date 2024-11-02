#include "eepch.h"
#include "graphics/misc/DPI.h"
#include "imguiProfiler/Profiler.h"
#include "logPanel.h"
#include "ui/iconsFontAwesome.h"
#include "ui/imguiManager.h"
#include "utils/debug/valueTester.h"
#include "utils/text/stringUtils.h"
#include <mutex>

namespace emerald {
	static const char* s_logLevelStrings[4] = { "Fatal", "Error", "Warning", "Info" };
	static constexpr uint32_t LOG_PANEL_MESSAGE_MAX_LENGTH = 512;
	static constexpr float LOG_PANEL_HEIGHT_RATIO_DEFAULT = 0.7f;
	static constexpr float LOG_PANEL_MIN_DETAIL_HEIGHT = 100.0f;

	LogPanel::LogPanel()
		: m_selectedLogLevel((uint32_t)LogLevel::INFO),
		m_logPanelHeightRatio(LOG_PANEL_HEIGHT_RATIO_DEFAULT),
		m_lastLogCount(0),
		m_selectedMessageIndex(-1),
		m_autoscroll(true),
		m_collapseLogs(true) {
		memset(m_searchString, 0, sizeof(m_searchString));
	}

	void LogPanel::drawButtons() {
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

		ImVec2 layout_size = ImVec2(ImGui::GetContentRegionAvail().x, 0.0f);
		ImGui::BeginHorizontal("LogButtons", layout_size);

		if (ImGui::Button("Clear")) Log::clearMessages();
		ImGui::SetItemTooltip("Clear all log messages");

		if (ImGui::ToggleButton("Collapse", &m_collapseLogs)) forceRefresh();
		ImGui::SetItemTooltip("Collapse duplicate log entries");

		ImGui::ToggleButton("Autoscroll", &m_autoscroll);
		ImGui::SetItemTooltip("Automatically scroll down if the panel is scrolled down");

		ImGui::Spring(1.0f, 0.0f);

		ImGui::SetNextItemWidth(DPI::getScale(200.0f));
		bool searchChanged = ImGui::InputTextWithHint("##LogPanelFilter", ICON_FA_SEARCH " Search...", m_searchString, sizeof(m_searchString), ImGuiInputTextFlags_EscapeClearsAll);
		if (ImGui::Button(ICON_FA_TIMES)) {
			memset(m_searchString, 0, sizeof(m_searchString));
			searchChanged = true;
		}
		if (searchChanged) forceRefresh();

		ImGui::SetNextItemWidth(DPI::getScale(100.0f));
		if (ImGui::BeginCombo("##LogLevel", s_logLevelStrings[m_selectedLogLevel], ImGuiComboFlags_PopupAlignLeft)) {
			ImGui::ItemRowsBackground();
			for (int i = 0; i < 4; i++) {
				bool isSelected = (m_selectedLogLevel == i);
				if (ImGui::Selectable(s_logLevelStrings[i], isSelected)) {
					m_selectedLogLevel = i;
					forceRefresh();
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SetItemTooltip("Filter log entries by log level");

		ImGui::Spring(0.0f, 0.0f);

		ImGui::EndHorizontal();
		ImGui::PopStyleVar();
	}

	// Draws the resizable splitter between log and detail panels
	void LogPanel::drawSplitter(float totalHeight, float logPanelHeight) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::InvisibleButton("LogPanelSplitter", ImVec2(ImGui::GetWindowWidth(), 4));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
		ImGui::PopStyleVar();

		if (ImGui::IsItemHovered() || ImGui::IsItemActive()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
		}

		if (ImGui::IsItemActive()) {
			m_logPanelHeightRatio += ImGui::GetIO().MouseDelta.y / totalHeight;
			m_logPanelHeightRatio = std::clamp(m_logPanelHeightRatio, 0.1f, 0.9f);
		}
	}

	// Draws the details panel for a selected log entry
	void LogPanel::drawDetailsPanel(float panelHeight) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
		ImGui::BeginChild("LogPanelDetails", ImVec2(0, panelHeight), true);
		ImGui::TextWrapped("%s %s", m_selectedMessage.m_timestamp.c_str(), m_selectedMessage.m_message.c_str());
		ImGui::TextWrapped("%s", m_selectedMessage.m_stackTrace.c_str());
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	void LogPanel::draw() {
		PROFILE_RENDER_SCOPE("LogPanel");

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);

		if (ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoNav)) {
			drawButtons();

			bool isAtBottom = false;
			float totalHeight = ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - 8;
			float logPanelHeight = totalHeight * m_logPanelHeightRatio;
			float detailsPanelHeight = totalHeight - logPanelHeight;

			// Ensure minimum height for details panel
			if (detailsPanelHeight < DPI::getScale(LOG_PANEL_MIN_DETAIL_HEIGHT)) {
				detailsPanelHeight = DPI::getScale(LOG_PANEL_MIN_DETAIL_HEIGHT);
				logPanelHeight = totalHeight - DPI::getScale(LOG_PANEL_MIN_DETAIL_HEIGHT);
			}

			ImGui::BeginChild("LogPanelMessages", ImVec2(0, logPanelHeight), true);
			if (m_autoscroll) isAtBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::ItemRowsBackground(18);

			ImGuiListClipper clipper;
			{
				std::lock_guard<std::mutex> lock(Log::getMessageMutex());

				// Rebuild cache if new logs or search criteria change
				if (m_lastLogCount != (uint32_t)Log::getMessages().size()) {
					m_lastLogCount = (uint32_t)Log::getMessages().size();
					m_collapsedLogs.clear();
					m_logMessageCounts.clear();

					for (const auto& entry : Log::getMessages()) {
						if (m_selectedLogLevel >= static_cast<uint32_t>(entry.m_level) &&
							(strlen(m_searchString) == 0 || stringUtils::toLower(entry.m_message).find(stringUtils::toLower(m_searchString)) != std::string::npos)) {

							size_t hash = std::hash<std::string>{}(entry.m_message);

							if (m_collapseLogs && m_logMessageCounts[hash]++ == 0) {
								m_collapsedLogs.push_back(&entry);
							} else if (!m_collapseLogs) {
								m_collapsedLogs.push_back(&entry);
							}
						}
					}
				}

				clipper.Begin((int)m_collapsedLogs.size());
				while (clipper.Step()) {
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
						const auto* entry = m_collapsedLogs[i];
						int count = m_logMessageCounts[std::hash<std::string>{}(entry->m_message)];

						ImVec4 color;
						switch (entry->m_level) {
							case LogLevel::INFO:    color = ImVec4(0.80f, 0.90f, 0.63f, 1.0f); break;
							case LogLevel::WARN:    color = ImVec4(1.00f, 0.72f, 0.50f, 1.0f); break;
							case LogLevel::ERROR:   color = ImVec4(0.75f, 0.25f, 0.25f, 1.0f); break;
							case LogLevel::FATAL:   color = ImVec4(1.00f, 0.00f, 0.00f, 1.0f); break;
						}
						ImGui::TextColored(color, "[%s] ", s_logLevelStrings[(uint32_t)entry->m_level]);

						ImGui::SameLine();
						char buf[LOG_PANEL_MESSAGE_MAX_LENGTH];
						sprintf_s(buf, "%s %s##%d", entry->m_timestamp.c_str(), entry->m_message.c_str(), i);

						if (ImGui::Selectable(buf, i == m_selectedMessageIndex, ImGuiSelectableFlags_None, ImVec2(0, DPI::getScale(18.0f)))) {
							m_selectedMessageIndex = i;
							m_selectedMessage = *entry;
						}

						//Select the item if it is navigated to using the keyboard
						if (ImGui::IsItemFocused()) {
							m_selectedMessageIndex = i;
							m_selectedMessage = *entry;
						}

						if (m_collapseLogs && count > 1) {
							ImGui::SameLine();

							char countBuf[32];
							sprintf_s(countBuf, "%d", count);
							ImVec2 buttonSize = ImGui::CalcTextSize(countBuf);

							buttonSize.x += DPI::getScale(16);
							buttonSize.y = 0;

							float availableWidth = ImGui::GetContentRegionAvail().x;
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availableWidth - buttonSize.x - DPI::getScale(15));

							ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, DPI::getScale(6.0f));
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(DPI::getScale(8.0f), 0));

							ImGui::BeginDisabled(true);
							ImGui::Button(countBuf, buttonSize);
							ImGui::EndDisabled();

							ImGui::PopStyleVar(2);
						}

					}
				}
				ImGui::Dummy(ImVec2(0, 2));
			}
			clipper.End();

			// Auto-scroll logic
			if (m_autoscroll && isAtBottom) {
				ImGui::SetScrollHereY(1.0f);
			}

			ImGui::PopStyleVar();
			ImGui::EndChild();

			// Splitter and details panel
			drawSplitter(totalHeight, logPanelHeight);
			drawDetailsPanel(detailsPanelHeight);
		}

		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void LogPanel::forceRefresh() {
		m_lastLogCount = 0;
	}
}