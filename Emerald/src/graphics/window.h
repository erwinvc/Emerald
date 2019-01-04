#pragma once

class Window {
private:
    GLFWwindow* m_window;
    String m_title;
    uint m_width, m_height;
    bool m_vSync;

    function<void(Event&)> EventCallback;

public:
    Window(String title, int width, int height, std::function<void(Event&)> eventCallback) : m_title(title), m_width(width), m_height(height), m_vSync(false), EventCallback(eventCallback) {
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
        if (!m_window)glfwTerminate();
        glfwSetWindowUserPointer(m_window, this);

        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
            Window* windowptr = (Window*)glfwGetWindowUserPointer(window);
            windowptr->SetWidth(width);
            windowptr->SetHeight(height);

            WindowResizeEvent event(width, height);
            windowptr->EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
            Window* windowptr = (Window*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            windowptr->EventCallback(event);
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Window* windowptr = (Window*)glfwGetWindowUserPointer(window);
        });
    }

    inline void SetWidth(uint width) { m_width = width; }
    inline void SetHeight(uint height) { m_height = height; }

    inline uint GetWidth() { return m_width; }
    inline uint GetHeight() { return m_height; }

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

    GLFWwindow* GetWindow() { return m_window; }

};