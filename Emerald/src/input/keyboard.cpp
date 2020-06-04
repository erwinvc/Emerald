#include "stdafx.h"

bool Keyboard::CheckImGuiControl() {
	if (m_overrideImGuiThisFrame) return false;
	return ImGui::GetCurrentContext()->NavWindow || ImGui::GetIO().WantCaptureKeyboard;
}

void Keyboard::Initialize(Window* window) {
    m_window = window;
    for (int i = 0; i < KEYSIZE; i++) {
        m_keyStates[i] = Key();
    }

    GetGLCallbackManager()->AddOnKeyCallback(this, &Keyboard::OnKey);
}

void Keyboard::Update() {
	m_imGuiControlThisFrame = CheckImGuiControl();
	m_overrideImGuiThisFrame = m_overrideImGuiNextFrame;
	m_overrideImGuiNextFrame = false;
}

void Keyboard::ResetKeyState(DWORD key) {
	if ((int)key < KEYSIZE) memset(&m_keyStates[key], 0, sizeof(m_keyStates[0]));
}

bool Keyboard::KeyDown(DWORD key) {
	if ((int)key >= KEYSIZE || CheckImGuiControl() || m_imGuiControlThisFrame) return false;
	return (GetTickCount() < m_keyStates[key].time + m_MAXDOWN) && !m_keyStates[key].m_isUpNow;
}

bool Keyboard::AnyKeyDown() {
	return KeyDown(m_lastKey);
}

bool Keyboard::KeyJustUp(DWORD key) {
	if ((int)key >= KEYSIZE || CheckImGuiControl() || m_imGuiControlThisFrame) return false;
	bool result = GetTickCount() < m_keyStates[key].time + m_NOWPERIOD && m_keyStates[key].m_isUpNow;
	if (result) ResetKeyState(key);
	return result;
}

bool Keyboard::AnyKeyJustUp() {
	return KeyJustUp(m_lastKey);
}

bool Keyboard::KeyJustDown(DWORD key) {
	if ((int)key >= KEYSIZE || CheckImGuiControl() || m_imGuiControlThisFrame) return false;
	return m_keyStates[key].m_justDown;
}

bool Keyboard::AnyKeyJustDown() {
	return KeyJustDown(m_lastKey);
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