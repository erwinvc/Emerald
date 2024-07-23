#include "eepch.h"
#include "window.h"

namespace emerald {
	const GLWindowCallbacks& GLWindowCallbacks::GLWindowCallbacks::getWindowCallbacks(GLFWwindow* wnd) {
		return *(GLWindowCallbacks*)glfwGetWindowUserPointer(wnd);
	}

	void GLWindowCallbacks::initialize(Window* window) {
		glfwSetWindowUserPointer(window->getHandle(), this);

		glfwSetWindowFocusCallback(window->getHandle(), [](GLFWwindow* wnd, int focussed) {
			GLWindowCallbacks* callbackManager = (GLWindowCallbacks*)glfwGetWindowUserPointer(wnd);
			for (auto& callback : getWindowCallbacks(wnd).m_onFocusCallbacks) {
				callback(focussed);
			}
			});

		glfwSetWindowSizeCallback(window->getHandle(), [](GLFWwindow* wnd, int width, int height) {
			for (auto& callback : getWindowCallbacks(wnd).m_onResizeCallbacks) {
				callback(width, height);
			}
			});

		glfwSetWindowCloseCallback(window->getHandle(), [](GLFWwindow* wnd) {
			for (auto& callback : getWindowCallbacks(wnd).m_onCloseCallbacks) {
				callback();
			}
			});

		glfwSetKeyCallback(window->getHandle(), [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
			for (auto& callback : getWindowCallbacks(wnd).m_onKeyCallbacks) {
				callback(key, scancode, action, mods);
			}
			});

		glfwSetMouseButtonCallback(window->getHandle(), [](GLFWwindow* wnd, int button, int action, int mods) {
			for (auto& callback : getWindowCallbacks(wnd).m_onMouseButtonCallbacks) {
				callback(button, action, mods);
			}
			});

		glfwSetCursorPosCallback(window->getHandle(), [](GLFWwindow* wnd, double xpos, double ypos) {
			for (auto& callback : getWindowCallbacks(wnd).m_onMousePosCallbacks) {
				callback(xpos, ypos);
			}
			});

		glfwSetScrollCallback(window->getHandle(), [](GLFWwindow* wnd, double xoffset, double yoffset) {
			for (auto& callback : getWindowCallbacks(wnd).m_onScrollCallbacks) {
				callback(xoffset, yoffset);
			}
			});

		glfwSetCharCallback(window->getHandle(), [](GLFWwindow* wnd, uint32_t character) {
			for (auto& callback : getWindowCallbacks(wnd).m_onKeyTypedCallbacks) {
				callback(character);
			}
			});
	}
}