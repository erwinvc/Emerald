#include "eepch.h"
#include "window.h"
#include "engine/events/eventSystem.h"
#include "engine/input/mouse.h"

namespace emerald {
	const GLWindowCallbacks& GLWindowCallbacks::GLWindowCallbacks::getWindowCallbacks(GLFWwindow* wnd) {
		return *(GLWindowCallbacks*)glfwGetWindowUserPointer(wnd);
	}

	void GLWindowCallbacks::initialize(Window* window) {
		glfwSetWindowUserPointer(window->handle(), this);

		glfwSetWindowFocusCallback(window->handle(), [](GLFWwindow* wnd, int focussed) {
			GLWindowCallbacks* callbackManager = (GLWindowCallbacks*)glfwGetWindowUserPointer(wnd);
			for (auto& callback : getWindowCallbacks(wnd).m_onFocusCallbacks) {
				callback(focussed);
			}
		});

		glfwSetWindowSizeCallback(window->handle(), [](GLFWwindow* wnd, int width, int height) {
			for (auto& callback : getWindowCallbacks(wnd).m_onResizeCallbacks) {
				callback(width, height);
			}
		});

		glfwSetWindowCloseCallback(window->handle(), [](GLFWwindow* wnd) {
			for (auto& callback : getWindowCallbacks(wnd).m_onCloseCallbacks) {
				callback();
			}
		});

		glfwSetKeyCallback(window->handle(), [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
			for (auto& callback : getWindowCallbacks(wnd).m_onKeyCallbacks) {
				callback(key, scancode, action, mods);
			}
		});

		glfwSetMouseButtonCallback(window->handle(), [](GLFWwindow* wnd, int button, int action, int mods) {
			for (auto& callback : getWindowCallbacks(wnd).m_onMouseButtonCallbacks) {
				callback(button, action, mods);
			}

			EventSystem::dispatch<MouseButtonEvent>((MouseButton)button, action, mods);
		});

		glfwSetCursorPosCallback(window->handle(), [](GLFWwindow* wnd, double xpos, double ypos) {
			for (auto& callback : getWindowCallbacks(wnd).m_onMousePosCallbacks) {
				callback(xpos, ypos);
			}

			EventSystem::dispatch<MousePositionEvent>((float)xpos, (float)ypos);
		});

		glfwSetScrollCallback(window->handle(), [](GLFWwindow* wnd, double xoffset, double yoffset) {
			for (auto& callback : getWindowCallbacks(wnd).m_onScrollCallbacks) {
				callback(xoffset, yoffset);
			}

			EventSystem::dispatch<MouseScrollEvent>((float)xoffset, (float)yoffset);
		});

		glfwSetCharCallback(window->handle(), [](GLFWwindow* wnd, uint32_t character) {
			for (auto& callback : getWindowCallbacks(wnd).m_onKeyTypedCallbacks) {
				callback(character);
			}
		});
	}
}