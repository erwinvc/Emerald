#pragma once
#include "imgui.h"

namespace emerald {
	class DPI {
	public:
		static void initialize();
		static void update();
		static float getScale() { return currentXScale; }

		template<typename T>
		static T getScale(T value) {
			return value * currentXScale;
		}

		template<>
		ImVec2 getScale<ImVec2>(ImVec2 value) {
			return ImVec2(value.x * currentXScale, value.y * currentXScale);
		}

	private:
		static inline float currentXScale = 1.0f;
	};
}