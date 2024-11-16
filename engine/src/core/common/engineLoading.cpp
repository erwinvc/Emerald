#include "eepch.h"
#include "engineLoading.h"
#include "ui/imguiManager.h"

namespace emerald {
	void EngineLoading::startLoading(const std::string& message, bool canCancel) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_canCancel = canCancel;
		m_loading = true;
		m_isCanceled = false;
		m_progress = 0.0f;
		m_message = message;
		m_taskDescription.clear();
	}

	void EngineLoading::stopLoading() {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_loading = false;
		m_progress = 1.0f;
		m_message.clear();
		m_taskDescription.clear();
	}

	void EngineLoading::setProgress(float progress) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_progress = progress;
	}

	void EngineLoading::setTaskDescription(const std::string& description) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_taskDescription = description;
	}

	float EngineLoading::getProgress() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_progress;
	}

	bool EngineLoading::isLoading() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_loading;
	}

	const std::string& EngineLoading::getMessage() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_message;
	}

	const std::string& EngineLoading::getTaskDescription() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_taskDescription;
	}

	void EngineLoading::draw() {
		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_loading) {
			ImGui::OpenPopup("Loading");
		}

		ImGui::PushStyleColor(ImGuiCol_Text, ImGuiManager::getSeverityColor(Severity::INFO));
		if (ImGui::BeginPopupModal("Loading", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImGui::PopStyleColor();

			ImGui::Text("%s", m_message.c_str());

			if (!m_taskDescription.empty()) {
				ImGui::Separator();
				ImGui::Text("Task: %s", m_taskDescription.c_str());
			}

			ImGui::ProgressBar(m_progress, ImVec2(-1.0f, 0.0f));

			if (m_canCancel && ImGui::Button("Cancel", ImVec2(-1, 0))) {
				m_isCanceled = true;
			}

			if (m_progress >= 1.0f) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		} else {
			ImGui::PopStyleColor();
		}
	}
}