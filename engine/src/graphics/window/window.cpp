#include "eepch.h"
#include "window.h"

namespace emerald {
	Window::Window(std::string title, int width, int height) : m_title(title), m_width(width), m_height(height) {
		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
		m_callbacks.initialize(this);
		m_callbacks.addOnResizeCallback(this, &Window::onResize);
		m_callbacks.addOnFocusCallback(this, &Window::onFocusEvent);
	}

	Window::~Window() { glfwDestroyWindow(m_window); }

	void Window::setVSync(bool toggle) {
		m_vSync = toggle;
		glfwSwapInterval(m_vSync);
	}

	void Window::setTitle(const std::string& title) { m_title = title; glfwSetWindowTitle(m_window, title.c_str()); }
	void Window::swapBuffers() { glfwSwapBuffers(m_window); }
	void Window::pollEvents() { glfwPollEvents(); }
	bool Window::shouldClose() { return glfwWindowShouldClose(m_window); }
	void Window::makeContextCurrent() { glfwMakeContextCurrent(m_window); }
	void Window::show() { glfwShowWindow(m_window); }

	bool Window::isMaximized() const { return (bool)glfwGetWindowAttrib(handle(), GLFW_MAXIMIZED); }
	void Window::minimize() { glfwIconifyWindow(m_window); }
	void Window::maximize() { glfwMaximizeWindow(m_window); }
	void Window::restore() { glfwRestoreWindow(m_window); }

	void Window::setWidth(uint32_t width) {
		m_width = width;
		glfwSetWindowSize(m_window, m_width, m_height);
	}
	void Window::setHeight(uint32_t height) {
		m_height = height;
		glfwSetWindowSize(m_window, m_width, m_height);
	}

	void Window::setLimits(uint32_t minWidth, uint32_t minHeight, uint32_t maxWidth, uint32_t maxHeight) {
		glfwSetWindowSizeLimits(handle(), minWidth, minHeight, maxWidth, maxHeight);
	}

	void Window::onResize(uint32_t width, uint32_t height) {
		m_width = width;
		m_height = height;
	}
}