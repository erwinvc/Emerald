#pragma once

#define BUTTONSSIZE 5

#define VK_MOUSE_LEFT	0
#define VK_MOUSE_MIDDLE	2
#define VK_MOUSE_RIGHT	1

class Mouse {
private:
    const int m_NOWPERIOD = 100, m_MAXDOWN = 600000, m_DOUBLECLICKPERIOD = 200;

    Vector2 m_position;
    Vector2 m_prevPosition;
    Vector2 m_usePosition;
    Vector2 m_delta;

    Vector2 m_scroll;

    struct Button {
        DWORD m_time;
        DWORD m_doubleClickTime;
        DWORD m_doubleClicked;
        bool m_isUpNow;
        bool m_justDown;
        Button() : m_time(0), m_doubleClickTime(0), m_doubleClicked(0), m_isUpNow(true), m_justDown(false) {}
    } m_buttonStates[5];

    Window* m_window;

public:
    int m_lastButton;

    void Update() {
        m_prevPosition = m_usePosition;
        m_usePosition = m_position;
        m_delta = m_prevPosition - m_usePosition;
    }

    inline Vector2& GetPosition() { return m_usePosition; }
    inline Vector2& GetDelta() { return m_delta; }

    void Initialize(Window* window);
    void EnableButton(int button, bool down);
    bool ButtonDown(DWORD button);
    void ResetButtonState(DWORD button);
    bool ButtonJustUp(DWORD button);
    bool ButtonJustDown(DWORD button);
    bool ButtonDoubleClicked(DWORD button);

    bool Mouse::MouseWithin(float x, float y, float width, float height);
    bool Mouse::MouseWithinCentered(float x, float y, float width, float height);
};

Mouse* GetMouse();

inline bool ButtonDown(DWORD button) {
    return GetMouse()->ButtonDown(button);
}
inline bool ButtonJustUp(DWORD button) {
    return GetMouse()->ButtonJustUp(button);
}

inline bool ButtonJustDown(DWORD button) {
    return GetMouse()->ButtonJustDown(button);
}
inline bool ButtonDoubleClicked(DWORD button) {
    return GetMouse()->ButtonDoubleClicked(button);
}