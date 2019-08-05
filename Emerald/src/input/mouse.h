#pragma once

#define BUTTONSSIZE 5

#define VK_MOUSE_LEFT	0
#define VK_MOUSE_MIDDLE	2
#define VK_MOUSE_RIGHT	1

class Mouse : public Singleton<Mouse> {
private:
    const int m_NOWPERIOD = 100, m_MAXDOWN = 600000, m_DOUBLECLICKPERIOD = 200;

    Vector2 m_position;
    Vector2 m_prevPosition;
    Vector2 m_usePosition;
    Vector2 m_delta;

    Vector2 m_scroll;

    struct Button {
        bool m_glIsUpNow;
        bool m_isUpNow;
        bool m_wasUp;
        bool m_justDown;
        bool m_justUp;
        Button() : m_glIsUpNow(true), m_isUpNow(true), m_wasUp(true), m_justDown(false), m_justUp(false) {}
    } m_buttonStates[5];

    void OnMouseButton(int button, int action, int mods);
    void OnMousePos(double xpos, double ypos);
    void OnScroll(double xoffset, double yoffset);

public:
    int m_lastButton;

    void Update() {
        for (int i = 0; i < 5; i++) {
            m_buttonStates[i].m_wasUp = m_buttonStates[i].m_isUpNow;
            m_buttonStates[i].m_isUpNow = m_buttonStates[i].m_glIsUpNow;
            m_buttonStates[i].m_justDown = m_buttonStates[i].m_wasUp && !m_buttonStates[i].m_isUpNow;
            m_buttonStates[i].m_justUp = !m_buttonStates[i].m_wasUp && m_buttonStates[i].m_isUpNow;
        }

        m_prevPosition = m_usePosition;
        m_usePosition = m_position;
        m_delta = m_prevPosition - m_usePosition;
    }

    inline Vector2& GetPosition() { return m_usePosition; }
    inline Vector2& GetDelta() { return m_delta; }

    void Initialize(Window* window);
    bool ButtonDown(DWORD button) { return !ImGui::GetIO().WantCaptureMouse && !m_buttonStates[button].m_isUpNow; }
    bool ButtonJustUp(DWORD button) { return !ImGui::GetIO().WantCaptureMouse && m_buttonStates[button].m_justUp; }
    bool ButtonJustDown(DWORD button) { return !ImGui::GetIO().WantCaptureMouse && m_buttonStates[button].m_justDown; }
    //bool ButtonDoubleClicked(DWORD button);

    bool Mouse::MouseWithin(float x, float y, float width, float height) {
        return Math::Within(m_usePosition.x, x, x + width) && Math::Within(m_usePosition.y, y, y + height);
    }

    bool Mouse::MouseWithinCentered(float x, float y, float width, float height) {
        return Math::Within(m_usePosition.x, x - width / 2, x + width / 2) && Math::Within(m_usePosition.y, y - height / 2, y + height / 2);
    }

};

static Mouse* GetMouse() { return Mouse::GetInstance(); }

inline bool ButtonDown(DWORD button) {
    return GetMouse()->ButtonDown(button);
}
inline bool ButtonJustUp(DWORD button) {
    return GetMouse()->ButtonJustUp(button);
}

inline bool ButtonJustDown(DWORD button) {
    return GetMouse()->ButtonJustDown(button);
}
//inline bool ButtonDoubleClicked(DWORD button) {
//    return GetMouse()->ButtonDoubleClicked(button);
//}