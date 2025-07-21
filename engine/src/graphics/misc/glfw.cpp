#include "eepch.h"
#include "glfw.h"
#define GLFW_TITLEBAR 0x00C2000D

namespace emerald {
	namespace GLFW {
		static bool s_initialized = false;

		void glfwErrorCallback(int error, const char* description) {
			Log::info("[GLFW] Error {0}: {1}", error, description);
		}

		void setWindowHints(const GLFWConfiguration& config) {
			glfwDefaultWindowHints();
			glfwWindowHint(GLFW_TITLEBAR, false);
			glfwWindowHint(GLFW_VISIBLE, config.visible);
			glfwWindowHint(GLFW_RESIZABLE, config.resizable);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.majorVersion);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.minorVersion);
			glfwWindowHint(GLFW_DEPTH_BITS, config.depthBits);

#ifdef EE_DEBUG
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		}

		GLFWmonitor* getWindowMonitor(GLFWwindow* window) {
			int windowX, windowY, windowWidth, windowHeight;
			glfwGetWindowPos(window, &windowX, &windowY);
			glfwGetWindowSize(window, &windowWidth, &windowHeight);

			int monitorCount;
			GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
			GLFWmonitor* bestMonitor = NULL;
			int bestOverlap = 0;

			for (int i = 0; i < monitorCount; i++) {
				int monitorX, monitorY;
				glfwGetMonitorPos(monitors[i], &monitorX, &monitorY);

				const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
				if (!mode)
					continue;

				int monitorWidth = mode->width;
				int monitorHeight = mode->height;

				int areaMinX = std::max(windowX, monitorX);
				int areaMinY = std::max(windowY, monitorY);
				int areaMaxX = std::min<int>(windowX + windowWidth, monitorX + monitorWidth);
				int areaMaxY = std::min<int>(windowY + windowHeight, monitorY + monitorHeight);

				int overlapArea = glm::max(0, areaMaxX - areaMinX) * glm::max(0, areaMaxY - areaMinY);

				if (overlapArea > bestOverlap) {
					bestOverlap = overlapArea;
					bestMonitor = monitors[i];
				}
			}

			return bestMonitor;
		}

		bool initialize(const GLFWConfiguration& config) {
			glfwSetErrorCallback(glfwErrorCallback);

			glfwInitHint(GLFW_WIN32_MESSAGES_IN_FIBER, true);

			if (!glfwInit()) return false;

			setWindowHints(config);

			s_initialized = true;
			return true;
		}

		void shutdown() {
			if (s_initialized) {
				glfwTerminate();
				s_initialized = false;
			}
		}
	}
}