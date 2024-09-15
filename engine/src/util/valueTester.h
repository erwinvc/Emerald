#pragma once
#include "input/keyboard.h"

namespace emerald {
	//Simple class to finetune values with keyboard input
	template<typename T>
	class ValueTester {
	private:
		T m_value;
		T m_incrementAmount;
	public:
		ValueTester() : m_value(T()), m_incrementAmount(1) {}
		ValueTester(T startValue, T incrementAmount) : m_value(startValue), m_incrementAmount(incrementAmount) {}

		T setValue(T newValue) { return m_value = newValue; }
		T getValue() const { return m_value; }

		void update() {
			if (Keyboard::keyMod(KeyMod::CONTROL)) {
				if (Keyboard::keyDown(Key::UP)) {
					m_value += m_incrementAmount;
				} else if (Keyboard::keyDown(Key::DOWN)) {
					m_value -= m_incrementAmount;
				}
			} else {
				if (Keyboard::keyJustDown(Key::UP)) {
					m_value += m_incrementAmount;
				} else if (Keyboard::keyJustDown(Key::DOWN)) {
					m_value -= m_incrementAmount;
				}
			}
			Log::info("Tester value: {}", m_value);
		}
	};
}