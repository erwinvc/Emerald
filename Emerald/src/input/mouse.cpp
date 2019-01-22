#include "stdafx.h"

static Mouse g_mouse;

void Mouse::Initialize(Window* window) {
    for (int i = 0; i < BUTTONSSIZE; i++) {
        m_buttonStates[i] = Button();
    }

    glfwSetMouseButtonCallback(window->GetWindow(), [](GLFWwindow* window, int button, int action, int mods) {
        GetMouse()->HandleButton(button, action, mods);
    });

    glfwSetCursorPosCallback(window->GetWindow(), [](GLFWwindow* window, double xpos, double ypos) {
        GetMouse()->m_position.x = (float)xpos;
        GetMouse()->m_position.y = (float)ypos;
    });
    glfwSetScrollCallback(window->GetWindow(), [](GLFWwindow* window, double xoffset, double yoffset) {
        GetMouse()->m_scroll.x = (float)xoffset;
        GetMouse()->m_scroll.y = (float)yoffset;
    });
}

void Mouse::HandleButton(int button, int action, int mods) {
    if (action == GLFW_RELEASE) {
        m_buttonStates[button].m_glIsUpNow = true;
    } else if (action == GLFW_PRESS) {
        GetMouse()->m_lastButton = button;
        m_buttonStates[button].m_glIsUpNow = false;
    }
}

Mouse* GetMouse() { return &g_mouse; }