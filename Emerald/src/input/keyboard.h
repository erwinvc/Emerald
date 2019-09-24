#pragma once

#define KEYSIZE 350

#define LSHIFT 340
#define LCTRL 341
#define LALT 342
#define RETURN 257

#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39

class Keyboard : public Singleton<Keyboard> {
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
		if ((int)key >= KEYSIZE || (ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureKeyboard)) return false;
		return (GetTickCount() < m_keyStates[key].time + m_MAXDOWN) && !m_keyStates[key].m_isUpNow;
	}

	bool AnyKeyDown() {
		return KeyDown(m_lastKey);
	}

	bool KeyJustUp(DWORD key) {
		if ((int)key >= KEYSIZE || (ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureKeyboard)) return false;
		bool result = GetTickCount() < m_keyStates[key].time + m_NOWPERIOD && m_keyStates[key].m_isUpNow;
		if (result) ResetKeyState(key);
		return result;
	}

	bool AnyKeyJustUp() {
		return KeyJustUp(m_lastKey);
	}

	bool KeyJustDown(DWORD key) {
		if ((int)key >= KEYSIZE || (ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureKeyboard)) return false;
		bool result = m_keyStates[key].m_justDown;
 		if (result) m_keyStates[key].m_justDown = false;
		return result;
	}

	bool AnyKeyJustDown() {
		return KeyJustDown(m_lastKey);
	}
};

static Keyboard* GetKeyboard() { return Keyboard::GetInstance(); }

inline bool KeyDown(DWORD key) { return GetKeyboard()->KeyDown(key); }
inline bool AnyKeyDown() { return GetKeyboard()->AnyKeyDown(); }

inline bool KeyJustUp(DWORD key) { return GetKeyboard()->KeyJustUp(key); }
inline bool AnyKeyJustUp() { return GetKeyboard()->AnyKeyJustUp(); }

inline bool KeyJustDown(DWORD key) { return GetKeyboard()->KeyJustDown(key); }
inline bool AnyKeyJustDown() { return GetKeyboard()->AnyKeyJustDown(); }