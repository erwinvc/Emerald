#include "eepch.h"
#include "core/application/application.h"
#include "DPI.h"
#include "glfw.h"
#include "graphics/window/window.h"
#include "ui/imguiManager.h"
#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")

namespace emerald {
	void DPI::initialize() {
		SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	}

	void DPI::update() {
		float xscale, yscale;
		GLFWmonitor* monitor = GLFW::getWindowMonitor(App->getWindow()->handle());
		if (monitor) {
			glfwGetMonitorContentScale(monitor, &xscale, &yscale);
			ImGuiIO& io = ImGui::GetIO();
			if (currentXScale != xscale) {
				currentXScale = xscale;
				io.DisplayFramebufferScale = ImVec2(xscale, yscale);
				ImGuiManager::rebuildFonts();
			}
		}
	}
}