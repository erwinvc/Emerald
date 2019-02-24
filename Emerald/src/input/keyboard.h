#pragma once

#define KEYSIZE 350

#define LSHIFT 340
#define LCTRL 341
#define LALT 342
#define RETURN 257

class Keyboard {
private:
    int m_lastKey;
    const int m_NOWPERIOD = 100;
    const int m_MAXDOWN = 600000; // ms

    struct Key {
        DWORD time;
        bool m_wasDownBefore;
        bool m_isUpNow;
        bool m_justUp;
        bool m_justDown;
        Key() : time(0), m_wasDownBefore(false), m_isUpNow(true), m_justUp(false), m_justDown(false) {}
    } m_keyStates[KEYSIZE];

    Window* m_window;

    void OnKey(int key, int scancode, int action, int mods);

public:
    void Initialize(Window* window);

    void ResetKeyState(DWORD key) {
        if ((int)key < KEYSIZE) memset(&m_keyStates[key], 0, sizeof(m_keyStates[0]));
    }

    bool KeyDown(DWORD key) {
        if (ImGui::GetIO().WantCaptureKeyboard) return false;
        return ((int)key < KEYSIZE) ? ((GetTickCount() < m_keyStates[key].time + m_MAXDOWN) && !m_keyStates[key].m_isUpNow) : false;
    }

    bool KeyJustUp(DWORD key) {
        if (ImGui::GetIO().WantCaptureKeyboard) return false;
        bool result = ((int)key < KEYSIZE) ? (GetTickCount() < m_keyStates[key].time + m_NOWPERIOD && m_keyStates[key].m_isUpNow) : false;
        if (result) ResetKeyState(key);
        return result;
    }

    bool KeyJustDown(DWORD key) {
        if (ImGui::GetIO().WantCaptureKeyboard) return false;
        bool result = GetTickCount() - m_NOWPERIOD < m_keyStates[key].time && !m_keyStates[key].m_isUpNow && m_keyStates[key].m_justDown;
        if (result) m_keyStates[key].m_justDown = false;
        return result;
    }
};

Keyboard* GetKeyboard();

inline bool KeyDown(DWORD key) {
    return GetKeyboard()->KeyDown(key);
}
inline bool KeyJustUp(DWORD key) {
    return GetKeyboard()->KeyJustUp(key);
}

inline bool KeyJustDown(DWORD key) {
    return GetKeyboard()->KeyJustDown(key);
}