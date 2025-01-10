#pragma once
#include "inputState.h"
#include "engine/events/events.h"

namespace emerald {
	enum class MouseButton {
		LEFT,
		RIGHT,
		MIDDLE,
		BACK,
		FORWARD,
		EXTRA1,
		EXTRA2,
		EXTRA3,
		_COUNT
	};

	class MouseButtonEvent : public Event {
	public:
		RTTI_DERIVED_CLASS_DECL(MouseButtonEvent, Event)

		MouseButtonEvent(MouseButton button, uint32_t action, uint32_t mods)
			: m_button(button), m_action(action)/*, m_mods(mods)*/ {
		}

		MouseButton getButton() const { return m_button; }
		//uint32_t getMods() const { return m_mods; }
		bool isPressed() const { return m_action == /*GLFW_PRESS*/ 1; }

	private:
		MouseButton m_button;
		uint32_t m_action;
		//uint32_t m_mods;
	};

	class MousePositionEvent : public Event {
	public:
		RTTI_DERIVED_CLASS_DECL(MousePositionEvent, Event)

		MousePositionEvent(float x, float y)
			: m_x(x), m_y(y) {
		}

		float getX() const { return m_x; }
		float getY() const { return m_y; }

	private:
		float m_x;
		float m_y;
	};

	class MouseScrollEvent : public Event {
	public:
		RTTI_DERIVED_CLASS_DECL(MouseScrollEvent, Event)

		MouseScrollEvent(float xOffset, float yOffset)
			: m_xOffset(xOffset), m_yOffset(yOffset) {
		}

		float getXOffset() const { return m_xOffset; }
		float getYOffset() const { return m_yOffset; }

	private:
		float m_xOffset;
		float m_yOffset;
	};

	class Mouse {
	public:
		static bool buttonDown(MouseButton button);
		static bool buttonUp(MouseButton button);
		static bool buttonJustDown(MouseButton button);
		static bool buttonJustUp(MouseButton button);

		static glm::vec2 position();
		static glm::vec2 positionDelta();
		static glm::vec2 scroll();

	private:
		friend class Application;

		static void mouseButtonCallback(MouseButtonEvent& e);
		static void mousePosCallback(MousePositionEvent& e);
		static void mouseScrollCallback(MouseScrollEvent& e);

		static void initialize();
		static void update();
	};
}