#include "eepch.h"
#include "glfw.h"
#define GLFW_TITLEBAR 0x00C2000D

namespace emerald {
	namespace GLFW {
		static bool s_initialized = false;

		void glfwErrorCallback(int error, const char* description) {
			Log::info("GLFW Error {0}: {1}", error, description);
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

		bool initialize(const GLFWConfiguration& config) {
			glfwSetErrorCallback(glfwErrorCallback);

			if (!glfwInit()) return false;

			setWindowHints(config);

			s_initialized = true;
			return true;
		}

		void terminate() {
			if (s_initialized) {
				glfwTerminate();
				s_initialized = false;
			}
		}
	}
}