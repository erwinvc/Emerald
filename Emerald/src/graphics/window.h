#pragma once

class Window {
private:
    GLFWwindow* m_window;
    String m_title;
    int m_width, m_height;
    bool m_vSync;

public:
    Window(String title, int width, int height) : m_title(title), m_width(width), m_height(height), m_vSync(false) {
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
        GetGLCallbackManager()->Initialize(this);
        if (!m_window)glfwTerminate();
    }

    inline void SetWidth(uint width) { m_width = width; }
    inline void SetHeight(uint height) { m_height = height; }

    inline int GetWidth() { return m_width; }
    inline int GetHeight() { return m_height; }

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

    void Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SetVSync(bool toggle) {
        glfwSwapInterval(toggle);
    }

    void SetTitle(const String& title) { glfwSetWindowTitle(m_window, title.c_str()); }

    GLFWwindow* GetHandle() { return m_window; }
};