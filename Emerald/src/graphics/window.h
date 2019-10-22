#pragma once

class Window {
private:
	GLFWwindow* m_window;
	String m_title;
	int m_width, m_height;
	bool m_vSync;
	bool m_focussed;

	void OnFocusEvent(int focus) {
		m_focussed = focus;
	}

public:
	Window(String title, int width, int height) : m_title(title), m_width(width), m_height(height), m_vSync(false) {
		m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
		GetGLCallbackManager()->Initialize(this);
		GetGLCallbackManager()->AddOnFocusCallback(this, &Window::OnFocusEvent);
		if (!m_window)glfwTerminate();
	}

	inline void SetWidth(uint width) { m_width = width; }
	inline void SetHeight(uint height) { m_height = height; }

	template<typename T>
	inline T GetWidth() { return (T)m_width; }

	template<typename T>
	inline T GetHeight() { return (T)m_height; }

	inline float GetAspect() { return (float)(m_width) / m_height; }

	~Window() {
		glfwTerminate();
	}

	bool GetFocussed() {
		return m_focussed;
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

	void Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void SetVSync(bool toggle) {
		glfwSwapInterval(toggle);
	}

	void SetIcon(const Icon& icon) {
		GL(glfwSetWindowIcon(GetHandle(), 1, icon.GetImage()));
	}

	void SetTitle(const String& title) { glfwSetWindowTitle(m_window, title.c_str()); }

	GLFWwindow* GetHandle() { return m_window; }
};