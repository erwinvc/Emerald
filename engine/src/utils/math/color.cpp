#include "eepch.h"
#include "color.h"
#include "random.h"

namespace emerald {
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
		float L = StaticRandom::getFloatInRange(min, 1.0f);  
		float a = StaticRandom::getFloatInRange(-0.5f, 0.5f);
		float b = StaticRandom::getFloatInRange(-0.5f, 0.5f);

		glm::vec3 rgb = oklabToRGB(L, a, b);

		return Color(glm::clamp(rgb.r, 0.0f, 1.0f), glm::clamp(rgb.g, 0.0f, 1.0f), glm::clamp(rgb.b, 0.0f, 1.0f), 1.0f);
	}

	Color Color::randomPrimary() {
		switch (StaticRandom::getIntInRange(0, 6)) {
			case 0: return red();
			case 1: return green();
			case 2: return blue();
			case 3: return yellow();
			case 4: return magenta();
			case 5: return cyan();
		}
		return white();
	}

	Color Color::mixSimple(const Color& col1, const Color& col2, float val) {
		return col1 * (1 - val) + col2 * val;
	}

	Color Color::mix(const Color& col1, const Color& col2, float val) {
		glm::vec3 lab1 = rgbToOklab(col1.r, col1.g, col1.b);
		glm::vec3 lab2 = rgbToOklab(col2.r, col2.g, col2.b);

		glm::vec3 labMix = glm::mix(lab1, lab2, val);

		glm::vec3 rgbMix = oklabToRGB(labMix.x, labMix.y, labMix.z);

		return Color(rgbMix.r, rgbMix.g, rgbMix.b, col1.a * (1.0f - val) + col2.a * val);
	}
}