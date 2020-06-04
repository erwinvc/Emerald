#pragma once

class Window {
private:
	GLFWwindow* m_window;
	String m_title;
	uint32 m_width, m_height;
	bool m_vSync;
	bool m_focussed;
	bool m_minimized = false;
	glm::ivec2 m_resizeBuffer = glm::ivec2(-1, -1);

	inline void OnFocusEvent(int focus) { m_focussed = focus; }

public:
	Window(String title, int width, int height);
	~Window() { glfwTerminate(); }

	inline void SetWidth(uint width) { m_width = width; }
	inline void SetHeight(uint height) { m_height = height; }
	inline uint GetWidth() { return m_width; }
	inline uint GetHeight() { return m_height; }
	inline float GetAspect() { return (float)(m_width) / m_height; }

	bool CheckResized();
	void SetVSync(bool toggle);
	void SetTitle(const String& title) { glfwSetWindowTitle(m_window, title.c_str()); }

	inline bool GetFocussed() { return m_focussed; }
	inline void SwapBuffers() { glfwSwapBuffers(m_window); }
	inline void PollEvents() { glfwPollEvents(); }
	inline bool ShouldClose() { return glfwWindowShouldClose(m_window); }
	inline void MakeContextCurrent() { glfwMakeContextCurrent(m_window); }
	inline void Show() { glfwShowWindow(m_window); }

	inline void ClearColor(Color& col) { glClearColor(col.R, col.G, col.B, col.A); }
	inline void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	inline bool GetVSync() { return m_vSync; }
	inline bool IsMinimized() { return m_minimized; }
	inline void SetIcon(const Icon& icon) { GL(glfwSetWindowIcon(GetHandle(), 1, icon.GetImage())); }
	inline void OnResize(int width, int height) { m_resizeBuffer = glm::ivec2(width, height); }

	inline GLFWwindow* GetHandle() { return m_window; }
};