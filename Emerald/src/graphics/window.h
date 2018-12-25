#pragma once

class Window {
private:
	String m_title;
	int m_width, m_height;
	GLFWwindow* m_window;

public:
	Window(String title, int width, int height) : m_title(title), m_width(width), m_height(height) {
		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
		if (!m_window)glfwTerminate();
		glfwSetWindowUserPointer(m_window, this);
	}

	~Window() {
		glfwTerminate();
	}

	void SwapBuffers() {
		glfwSwapBuffers(m_window);
	}

	void PollEvents() {
		glfwPollEvents();
	}

	bool ShouldClose() {
		return glfwWindowShouldClose(m_window);
	}

	void MakeContextCurrent() {
		glfwMakeContextCurrent(m_window);
	}

	void Show() {
		glfwShowWindow(m_window);
	}

	void ClearColor(Color& col) {
		glClearColor(col.R, col.G, col.B, col.A);
	}

	void SetVSync(bool toggle)
	{
		glfwSwapInterval(toggle);
	}

	GLFWwindow* GetWindow() {return m_window; }
};