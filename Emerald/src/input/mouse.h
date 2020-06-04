#pragma once

#define BUTTONSSIZE 5

#define VK_MOUSE_LEFT	0
#define VK_MOUSE_MIDDLE	2
#define VK_MOUSE_RIGHT	1

class Mouse : public Singleton<Mouse> {
private:
	const int m_NOWPERIOD = 100, m_MAXDOWN = 600000, m_DOUBLECLICKPERIOD = 200;
	Window* m_window;
	bool m_locked = false;
	bool m_imGuiControlThisFrame = false;
	bool m_overrideImGuiThisFrame = false;
	bool m_overrideImGuiNextFrame = false;
	
	glm::vec2 m_position;
	glm::vec2 m_prevPosition;
	glm::vec2 m_usePosition;
	glm::vec2 m_rawDelta;
	glm::vec2 m_delta;

	glm::vec2 m_scroll;
	glm::vec2 m_useScroll;

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
	float m_sensitivity = 0.5f;
	void Update();

	bool CheckImGuiControl();
	void Initialize(Window* window);
	bool MouseWithin(float x, float y, float width, float height);
	bool MouseWithinCentered(float x, float y, float width, float height);
	void SetPosition(glm::vec2& position);

	void SetLocked(bool locked) { m_locked = locked; }
	bool IsLocked() { return m_locked; }
	void OverrideImGuiCapture() { m_overrideImGuiNextFrame = true; }
	
	inline bool ButtonDown(DWORD button) { return !CheckImGuiControl() && !m_imGuiControlThisFrame && !m_buttonStates[button].m_isUpNow; }
	inline bool ButtonJustUp(DWORD button) { return !CheckImGuiControl() && !m_imGuiControlThisFrame && m_buttonStates[button].m_justUp; }
	inline bool ButtonJustDown(DWORD button) { return !CheckImGuiControl() && !m_imGuiControlThisFrame && m_buttonStates[button].m_justDown; }
	
	inline const glm::vec2& GetPosition() { return m_usePosition; }
	inline const glm::vec2& GetDelta() { return m_delta; }
	inline const glm::vec2& GetScroll() { return m_useScroll; }
};

inline Mouse* GetMouse() { return Mouse::GetInstance(); }

inline bool ButtonDown(DWORD button) {
	return GetMouse()->ButtonDown(button);
}
inline bool ButtonJustUp(DWORD button) {
	return GetMouse()->ButtonJustUp(button);
}

inline bool ButtonJustDown(DWORD button) {
	return GetMouse()->ButtonJustDown(button);
}
