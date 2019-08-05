#include "stdafx.h"

void Keyboard::Initialize(Window* window) {
    m_window = window;
    for (int i = 0; i < KEYSIZE; i++) {
        m_keyStates[i] = Key();
    }

    GetGLCallbackManager()->AddOnKeyCallback(this, &Keyboard::OnKey);
}

void Keyboard::OnKey(int key, int scancode, int action, int mods) {
    if ((int)key < KEYSIZE) {
        GetKeyboard()->m_keyStates[key].time = GetTickCount();
        GetKeyboard()->m_keyStates[key].m_wasDownBefore = GetKeyboard()->m_keyStates[key].m_justDown;
        if (action == GLFW_RELEASE) {
            GetKeyboard()->m_keyStates[key].m_isUpNow = true;
        } else if (action == GLFW_PRESS) {
            GetKeyboard()->m_keyStates[key].m_isUpNow = false;
        }
        if (!GetKeyboard()->m_keyStates[key].m_wasDownBefore) GetKeyboard()->m_keyStates[key].m_justDown = (action == GLFW_PRESS);
        if (action == GLFW_PRESS) GetKeyboard()->m_lastKey = key;
    }
}