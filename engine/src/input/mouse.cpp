#include "eepch.h"
#include "mouse.h"

namespace emerald {
	enum class MouseEventType {
		Button,
		Position,
		Scroll
	};

	struct MouseButtonEvent {
		uint32_t button;
		uint32_t action;
		uint32_t mods;
	};

	struct MousePositionEvent {
		double x;
		double y;
	};

	struct MouseScrollEvent {
		double xOffset;
		double yOffset;
	};

	struct MouseEvent {
		MouseEventType type;
		union {
			MouseButtonEvent button;
			MousePositionEvent position;
			MouseScrollEvent scroll;
		};
	};

	static InputState s_buttons[(uint32_t)MouseButton::_COUNT];
	static glm::vec2 m_position(0.0f);
	static glm::vec2 m_prevPosition(0.0f);
	static glm::vec2 m_positionDelta(0.0f);
	static glm::vec2 m_scroll(0.0f);

	static std::queue<MouseEvent> s_eventQueue;

	void Mouse::mouseButtonCallback(uint32_t button, uint32_t action, uint32_t mods) {
		MouseEvent event;
		event.type = MouseEventType::Button;
		event.button = MouseButtonEvent{ button, action, mods };
		s_eventQueue.push(event);
	}

	void Mouse::mousePosCallback(double x, double y) {
		MouseEvent event;
		event.type = MouseEventType::Position;
		event.position = MousePositionEvent{ x, y };
		s_eventQueue.push(event);
	}

	void Mouse::mouseScrollCallback(double xOffset, double yOffset) {
		MouseEvent event;
		event.type = MouseEventType::Scroll;
		event.scroll = MouseScrollEvent{ xOffset, yOffset };
		s_eventQueue.push(event);
	}

	void Mouse::update() {
		m_positionDelta = glm::vec2(0.0f);

		// Process all queued events
		for (int button = 0; button < (uint32_t)MouseButton::_COUNT; button++) {
			if (s_buttons[button] == InputState::JUSTDOWN) {
				s_buttons[button] = InputState::DOWN;
			} else if (s_buttons[button] == InputState::JUSTUP) {
				s_buttons[button] = InputState::UP;
			}
		}

		// Process all queued events
		while (!s_eventQueue.empty()) {
			MouseEvent event = s_eventQueue.front();
			s_eventQueue.pop();

			switch (event.type) {
				case MouseEventType::Button:
					if (event.button.action == GLFW_PRESS) {
						if (s_buttons[event.button.button] != InputState::DOWN) {
							s_buttons[event.button.button] = InputState::JUSTDOWN;
						}
					} else if (event.button.action == GLFW_RELEASE) {
						if (s_buttons[event.button.button] == InputState::DOWN || s_buttons[event.button.button] == InputState::JUSTDOWN) {
							s_buttons[event.button.button] = InputState::JUSTUP;
						}
					}
					break;

				case MouseEventType::Position:
					m_prevPosition = m_position;
					m_position.x = static_cast<float>(event.position.x);
					m_position.y = static_cast<float>(event.position.y);
					m_positionDelta = m_position - m_prevPosition;
					break;

				case MouseEventType::Scroll:
					m_scroll.x = (float)event.scroll.xOffset;
					m_scroll.y = (float)event.scroll.yOffset;
					break;
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