#pragma once

#define KEYSIZE 350

#define LSHIFT 340
#define LCTRL 341
#define LALT 342
#define RETURN 257

class Keyboard : public Singleton<Keyboard> {
private:
	int m_lastKey;
	const int m_NOWPERIOD = 100;
	const int m_MAXDOWN = 600000;
	bool m_imGuiControlThisFrame = false;
	bool m_overrideImGuiThisFrame = false;
	bool m_overrideImGuiNextFrame = false;

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

	bool CheckImGuiControl();

public:
	void Initialize(Window* window);
	void Update();
	void ResetKeyState(DWORD key);
	bool KeyDown(DWORD key);
	bool AnyKeyDown();
	bool KeyJustUp(DWORD key);
	bool AnyKeyJustUp();
	bool KeyJustDown(DWORD key);
	bool AnyKeyJustDown();

	inline void OverrideImGuiCapture() { m_overrideImGuiNextFrame = true; }
};

static Keyboard* GetKeyboard() { return Keyboard::GetInstance(); }

inline bool KeyDown(DWORD key) { return GetKeyboard()->KeyDown(key); }
inline bool AnyKeyDown() { return GetKeyboard()->AnyKeyDown(); }

inline bool KeyJustUp(DWORD key) { return GetKeyboard()->KeyJustUp(key); }
inline bool AnyKeyJustUp() { return GetKeyboard()->AnyKeyJustUp(); }

inline bool KeyJustDown(DWORD key) { return GetKeyboard()->KeyJustDown(key); }
inline bool AnyKeyJustDown() { return GetKeyboard()->AnyKeyJustDown(); }