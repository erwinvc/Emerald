#pragma once
#include "inputState.h"

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

		static void mouseButtonCallback(uint32_t button, uint32_t action, uint32_t mods);
		static void mousePosCallback(double x, double y);
		static void mouseScrollCallback(double x, double y);

		static void update();
	};
}