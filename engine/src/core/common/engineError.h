#pragma once
#include <string>
#include <source_location>
#include "utils/core/log.h"
#include "engine/events/events.h"
#include "engine/events/eventSystem.h"
#include "utils/datastructures/asyncStack.h"
#include "ui/imguiManager.h"

namespace emerald {
	class EngineError {
	public:
		static void raise(Severity severity, const std::string& message, const std::string& subMessage = "", const std::source_location& location = std::source_location::current()) {
			s_stack.push(Error(severity, message, subMessage, location));
			Log::error("{}", std::format("{}: {}", message, subMessage));
			EventSystem::dispatch<ErrorEvent>(message, subMessage);
		}

		static bool hasErrors() {
			return !s_stack.empty();
		}

		static void draw() {
			showProjectPopup(hasErrors());
		}

		static void showProjectPopup(bool open) {
			if (open) {
				ImGui::OpenPopup("Warning");
			}

			static Error error;
			s_stack.peek(error);

			ImGui::PushStyleColor(ImGuiCol_Text, ImGuiManager::getSeverityColor(error.m_severity));
			ImGui::SetNextWindowSize(ImVec2(450, 0));
			if (ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
				ImGui::PopStyleColor();

				ImGui::BeginVertical("test");

				ImGui::Text("%s", error.m_message.c_str());

				if (error.m_subMessage.length() > 0) {
					ImGui::Separator();
					ImGui::TextWrapped("%s", error.m_subMessage.c_str());
				}
				ImGui::Spring();
				if (ImGui::Button("Close", ImVec2(-1, 0))) {
					s_stack.pop();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndVertical();
				ImGui::EndPopup();
			} else {
				ImGui::PopStyleColor();
			}
		}

	private:
		struct Error {
			Severity m_severity = Severity::FATAL;
			std::string m_message = "";
			std::string m_subMessage = "";
			std::source_location m_location;

			Error() = default;
			Error(Severity severity, const std::string& message, const std::string& subMessage, const std::source_location& location) : m_severity(severity), m_message(message), m_subMessage(subMessage), m_location(location) {}
		};

		static inline AsyncStack<Error> s_stack;
	};
}