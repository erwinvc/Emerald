#include "stdafx.h"

static Mouse g_mouse;

void Mouse::Initialize(Window* window) {
    for (int i = 0; i < BUTTONSSIZE; i++) {
        m_buttonStates[i] = Button();
    }

    GetGLCallbackManager()->AddOnMouseButtonCallback(this, &Mouse::OnMouseButton);
    GetGLCallbackManager()->AddOnMousePosCallback(this, &Mouse::OnMousePos);
    GetGLCallbackManager()->AddOnScrollCallback(this, &Mouse::OnScroll);
}

void Mouse::OnMouseButton(int button, int action, int mods) {
    if (action == GLFW_RELEASE) {
        m_buttonStates[button].m_glIsUpNow = true;
    } else if (action == GLFW_PRESS) {
        GetMouse()->m_lastButton = button;
        m_buttonStates[button].m_glIsUpNow = false;
    }
}

void Mouse::OnMousePos(double xpos, double ypos) {
    GetMouse()->m_position.x = (float)xpos;
    GetMouse()->m_position.y = (float)ypos;
}


void Mouse::OnScroll(double xoffset, double yoffset) {
    GetMouse()->m_scroll.x = (float)xoffset;
    GetMouse()->m_scroll.y = (float)yoffset;
}