#include "eepch.h"
#include "mouse.h"
#include "engine/events/events.h"
#include "engine/events/eventSystem.h"

namespace emerald {
	RTTI_CLASS_DEF(MouseButtonEvent);
	RTTI_CLASS_DEF(MousePositionEvent);
	RTTI_CLASS_DEF(MouseScrollEvent);

	static InputState s_buttons[(uint32_t)MouseButton::_COUNT];
	static glm::vec2 m_position(0.0f);
	static glm::vec2 m_prevPosition(0.0f);
	static glm::vec2 m_positionDelta(0.0f);
	static glm::vec2 m_scroll(0.0f);

	void Mouse::mouseButtonCallback(MouseButtonEvent& e) {
		if (e.isPressed()) {
			if (s_buttons[(uint32_t)e.getButton()] != InputState::DOWN) {
				s_buttons[(uint32_t)e.getButton()] = InputState::JUSTDOWN;
			}
		} else {
			if (s_buttons[(uint32_t)e.getButton()] == InputState::DOWN || s_buttons[(uint32_t)e.getButton()] == InputState::JUSTDOWN) {
				s_buttons[(uint32_t)e.getButton()] = InputState::JUSTUP;
			}
		}
	}

	void Mouse::mousePosCallback(MousePositionEvent& e) {
		m_prevPosition = m_position;
		m_position.x = e.getX();
		m_position.y = e.getY();
		m_positionDelta = m_position - m_prevPosition;
	}

	void Mouse::mouseScrollCallback(MouseScrollEvent& e) {
		m_scroll.x = e.getXOffset();
		m_scroll.y = e.getYOffset();
	}


	void Mouse::initialize() {
		EventSystem::subscribe<MouseButtonEvent>(&Mouse::mouseButtonCallback);
		EventSystem::subscribe<MousePositionEvent>(&Mouse::mousePosCallback);
		EventSystem::subscribe<MouseScrollEvent>(&Mouse::mouseScrollCallback);
	}
	void Mouse::update() {
		m_positionDelta = glm::vec2(0.0f);

		for (int button = 0; button < (uint32_t)MouseButton::_COUNT; button++) {
			if (s_buttons[button] == InputState::JUSTDOWN) {
				s_buttons[button] = InputState::DOWN;
			} else if (s_buttons[button] == InputState::JUSTUP) {
				s_buttons[button] = InputState::UP;
			}
		}
	}

	bool Mouse::buttonDown(MouseButton button) {
		return s_buttons[(uint32_t)button] == InputState::DOWN || s_buttons[(uint32_t)button] == InputState::JUSTDOWN;
	}

	bool Mouse::buttonUp(MouseButton button) {
		return s_buttons[(uint32_t)button] == InputState::UP;
	}

	bool Mouse::buttonJustDown(MouseButton button) {
		return s_buttons[(uint32_t)button] == InputState::JUSTDOWN;
	}

	bool Mouse::buttonJustUp(MouseButton button) {
		return s_buttons[(uint32_t)button] == InputState::JUSTUP;
	}

	glm::vec2 Mouse::position() {
		return m_position;
	}

	glm::vec2 Mouse::positionDelta() {
		return m_positionDelta;
	}

	glm::vec2 Mouse::scroll() {
		return m_scroll;
	}
}