#include "stdafx.h"

static Mouse g_mouse;

void Mouse::Initialize(Window* window) {
    m_window = window;
    for (int i = 0; i < BUTTONSSIZE; i++) {
        m_buttonStates[i] = Button();
    }

    glfwSetMouseButtonCallback(m_window->GetWindow(), [](GLFWwindow* window, int button, int action, int mods) {
        if (action == GLFW_RELEASE) {
            GetMouse()->m_buttonStates[button].m_time = GetTickCount();
            GetMouse()->m_buttonStates[button].m_isUpNow = true;
        } else if (action == GLFW_PRESS) {
            GetMouse()->m_lastButton = button;
            GetMouse()->m_buttonStates[button].m_time = GetTickCount();
            GetMouse()->m_buttonStates[button].m_isUpNow = false;
        }
    });
    glfwSetCursorPosCallback(m_window->GetWindow(), [](GLFWwindow* window, double xpos, double ypos) {
        GetMouse()->m_position.x = (float)xpos;
        GetMouse()->m_position.y = (float)ypos;
    });
    glfwSetScrollCallback(m_window->GetWindow(), [](GLFWwindow* window, double xoffset, double yoffset) {
        GetMouse()->m_scroll.x = (float)xoffset;
        GetMouse()->m_scroll.y = (float)yoffset;
    });
}
void Mouse::EnableButton(int button, bool down) {
    m_buttonStates[button].m_doubleClicked = (button < 5 && m_buttonStates[button].m_isUpNow && GetTickCount() < m_buttonStates[button].m_time + m_DOUBLECLICKPERIOD);
    m_buttonStates[button].m_time = GetTickCount();
    m_buttonStates[button].m_isUpNow = !down;
    m_buttonStates[button].m_justDown = down;
}

bool Mouse::ButtonDown(DWORD button) {
    if (ImGui::GetIO().WantCaptureMouse) return false;
    return (button < 5) ? ((GetTickCount() < m_buttonStates[button].m_time + m_MAXDOWN) && !m_buttonStates[button].m_isUpNow) : false;
}

void Mouse::ResetButtonState(DWORD button) {
    if (button < 5) m_buttonStates[button] = Button();
}

bool Mouse::ButtonJustUp(DWORD button) {
    if (ImGui::GetIO().WantCaptureMouse) return false;
    const auto result = (button < 5) ? (GetTickCount() < m_buttonStates[button].m_time + m_NOWPERIOD && m_buttonStates[button].m_isUpNow) : false;
    if (result) ResetButtonState(button);
    return result;
}

bool Mouse::ButtonJustDown(DWORD button) {
    if (ImGui::GetIO().WantCaptureMouse) return false;
    bool result = (button < 5) ? (GetTickCount() - m_NOWPERIOD < m_buttonStates[button].m_time && !m_buttonStates[button].m_isUpNow && m_buttonStates[button].m_justDown) : false;
    if (result) m_buttonStates[button].m_justDown = false;
    return result;
}

bool Mouse::ButtonDoubleClicked(DWORD button) {
    if (ImGui::GetIO().WantCaptureMouse) return false;
    bool result = (button < 5) ? ((GetTickCount() < m_buttonStates[button].m_time + m_NOWPERIOD) && m_buttonStates[button].m_doubleClicked) : false;
    if (result) ResetButtonState(button);
    return result;
}

bool Mouse::MouseWithin(float x, float y, float width, float height) {
    return Math::Within(m_usePosition.x, x, x + width) && Math::Within(m_usePosition.y, y, y + height);
}

bool Mouse::MouseWithinCentered(float x, float y, float width, float height) {
    return Math::Within(m_usePosition.x, x - width / 2, x + width / 2) && Math::Within(m_usePosition.y, y - height / 2, y + height / 2);
}

Mouse* GetMouse() { return &g_mouse; }