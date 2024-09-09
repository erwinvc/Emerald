#include "eepch.h"
#include "keyboard.h"
#include "inputState.h"

namespace emerald {
	struct KeyEvent {
		uint32_t key;
		uint32_t action;
		uint32_t mods;
	};

	static InputState s_keyStates[(uint32_t)Key::_COUNT] = { InputState::UP };
	static bool s_keyModStates[(uint32_t)KeyMod::_COUNT] = { false };
	static std::queue<KeyEvent> s_eventQueue;

	void Keyboard::keyCallback(uint32_t key, uint32_t scancode, uint32_t action, uint32_t mods) {
		if (key >= (uint32_t)Key::_COUNT) {
			Log::error("[Keyboard] Unhandled key {}", key);
			return;
		}
		s_eventQueue.push(KeyEvent{ key, action, mods });
	}

	void Keyboard::update() {
		// Update key states for the next frame
		for (int key = 0; key < (uint32_t)Key::_COUNT; key++) {
			if (s_keyStates[key] == InputState::JUSTDOWN) {
				s_keyStates[key] = InputState::DOWN;
			} else if (s_keyStates[key] == InputState::JUSTUP) {
				s_keyStates[key] = InputState::UP;
			} else if (s_keyStates[key] == InputState::REPEAT) {
				s_keyStates[key] = InputState::DOWN;
			}
		}

		// Process all queued events
		while (!s_eventQueue.empty()) {
			KeyEvent event = s_eventQueue.front();
			s_eventQueue.pop();

			if (event.action == GLFW_PRESS) {
				if (s_keyStates[event.key] != InputState::DOWN && s_keyStates[event.key] != InputState::REPEAT) {
					s_keyStates[event.key] = InputState::JUSTDOWN;
				}
			} else if (event.action == GLFW_RELEASE) {
				if (s_keyStates[event.key] == InputState::DOWN || s_keyStates[event.key] == InputState::JUSTDOWN || s_keyStates[event.key] == InputState::REPEAT) {
					s_keyStates[event.key] = InputState::JUSTUP;
				}
			} else if (event.action == GLFW_REPEAT) {
				s_keyStates[event.key] = InputState::REPEAT;
			}

			// Update modifier key states
			s_keyModStates[(uint32_t)KeyMod::CONTROL] = (event.mods & GLFW_MOD_CONTROL) != 0;
			s_keyModStates[(uint32_t)KeyMod::SHIFT] = (event.mods & GLFW_MOD_SHIFT) != 0;
			s_keyModStates[(uint32_t)KeyMod::ALT] = (event.mods & GLFW_MOD_ALT) != 0;
			s_keyModStates[(uint32_t)KeyMod::SUPER] = (event.mods & GLFW_MOD_SUPER) != 0;
		}
	}

	bool Keyboard::keyDown(Key key) {
		return s_keyStates[(uint32_t)key] == InputState::DOWN || s_keyStates[(uint32_t)key] == InputState::JUSTDOWN || s_keyStates[(uint32_t)key] == InputState::REPEAT;
	}

	bool Keyboard::keyUp(Key key) {
		return s_keyStates[(uint32_t)key] == InputState::UP;
	}

	bool Keyboard::keyJustDown(Key key) {
		return s_keyStates[(uint32_t)key] == InputState::JUSTDOWN;
	}

	bool Keyboard::keyJustUp(Key key) {
		return s_keyStates[(uint32_t)key] == InputState::JUSTUP;
	}

	bool Keyboard::keyRepeat(Key key) {
		return s_keyStates[(uint32_t)key] == InputState::REPEAT;
	}

	bool Keyboard::keyMod(KeyMod mod) {
		return s_keyModStates[(uint32_t)mod];
	}
}