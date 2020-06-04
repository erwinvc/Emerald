#include "stdafx.h"

Window::Window(String title, int width, int height) : m_title(title), m_width(width), m_height(height), m_vSync(false) {
	m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
	GetGLCallbackManager()->Initialize(this);
	GetGLCallbackManager()->AddOnFocusCallback(this, &Window::OnFocusEvent);
	if (!m_window)glfwTerminate();
}

bool Window::CheckResized() {
	if (m_resizeBuffer.x != -1 || m_resizeBuffer.y != -1) {
		m_width = m_resizeBuffer.x;
		m_height = m_resizeBuffer.y;
		m_resizeBuffer.x = -1;
		m_resizeBuffer.y = -1;
		if (m_height == 0 || m_width == 0) m_minimized = true;
		else m_minimized = false;
		return true;
	}
	return false;
}

void Window::SetVSync(bool toggle) {
	m_vSync = toggle;
	glfwSwapInterval(m_vSync);
}