#include "eepch.h"
#include "profilerWindow.h"
#include "editor.h"
#include "ui/imguiManager.h"
#include "imguiProfiler/Profiler.h"

namespace emerald{
	void ProfilerWindow::draw(){
		if (EditorWindows.profiler) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(800, 300));
			ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
			if (ImGui::Begin("Profiler", &EditorWindows.profiler)) {
				ImGuiManager::pushFont(ImGUIFont::INTER);
				DrawProfilerHUD();
				ImGuiManager::popFont();
			}
			ImGui::PopStyleVar();
			ImGui::End();

			if (!EditorWindows.profiler) {
				PROFILE_DISABLE();
			}
		}
	}}