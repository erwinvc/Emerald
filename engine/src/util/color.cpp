#include "eepch.h"
#include "color.h"
#include <glm/common.hpp>

namespace emerald {
	Color8 Color::toColor8() {
		uint8_t _r = (uint8_t)glm::round(r * 255);
		uint8_t _g = (uint8_t)glm::round(g * 255);
		uint8_t _b = (uint8_t)glm::round(b * 255);
		uint8_t _a = (uint8_t)glm::round(a * 255);
		return Color8(_r, _g, _b, _a);
	}

	void Color::clamp() {
		if (r < 0.0f) r = 0.0f;
		if (g < 0.0f) g = 0.0f;
		if (b < 0.0f) b = 0.0f;
		if (a < 0.0f) a = 0.0f;

		if (r > 1.0f) r = 1.0f;
		if (g > 1.0f) g = 1.0f;
		if (b > 1.0f) b = 1.0f;
		if (a > 1.0f) a = 1.0f;
	}

	float Color::getRGBCombined() { return r + g + b; }

	Color Color::random(float min) {
		//float rr = Random::nextFloat(0, 1);
		//float gg = Random::nextFloat(0, 1);
		//float bb = Random::nextFloat(0, 1);
		////if (rr + gg + bb < min) return Random(min);
		//return Color(rr, gg, bb);
		return white();
	}

	Color Color::randomPrimary() {
		//switch (Random::nextInt(5)) {
		//	case 0: return red();
		//	case 1: return green();
		//	case 2: return blue();
		//	case 3: return yellow();
		//	case 4: return magenta();
		//	case 5: return cyan();
		//}
		return white();
	}

	Color Color::mix(const Color& col1, const Color& col2, float val) {
		return col1 * (1 - val) + col2 * val;
	}
}