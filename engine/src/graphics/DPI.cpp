#include "eepch.h"
#include "DPI.h"
#include <ShellScalingAPI.h>
#include "glfw.h"
#include "application.h"
#include "imgui.h"
#include "graphics/window.h"
#include "ui/imguiManager.h"
#pragma comment(lib, "Shcore.lib")

namespace emerald {
	void DPI::initialize() {
		SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	}

	void DPI::update() {
		float xscale, yscale;
		glfwGetMonitorContentScale(GLFW::getWindowMonitor(App->getWindow()->handle()), &xscale, &yscale);

		ImGuiIO& io = ImGui::GetIO();
		if (currentXScale != xscale) {
			currentXScale = xscale;
			io.DisplayFramebufferScale = ImVec2(xscale, yscale);
			ImGuiManager::rebuildFonts();
		}
	}
}