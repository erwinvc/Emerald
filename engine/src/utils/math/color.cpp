#include "eepch.h"
#include "color.h"
#include "random.h"

namespace emerald {
	namespace emerald::ColorSpace {
		inline glm::vec3 generateRandomOklab(float minLuminance) noexcept {
			minLuminance = std::clamp(minLuminance, 0.0f, 1.0f);

			return glm::vec3(
				StaticRandom::getFloatInRange(minLuminance, 1.0f),
				StaticRandom::getFloatInRange(-0.4f, 0.4f),
				StaticRandom::getFloatInRange(-0.4f, 0.4f)
			);
		}

		inline glm::vec3 generateRandomHSV(float minSaturation, float minValue) noexcept {
			return glm::vec3(
				StaticRandom::getFloat(),
				StaticRandom::getFloatInRange(minSaturation, 1.0f),
				StaticRandom::getFloatInRange(minValue, 1.0f)
			);
		}

		inline glm::vec3 rgbToOklab(const glm::vec3& rgb) noexcept {
			const glm::vec3 lrgb = glm::pow(rgb, glm::vec3(2.4f));

			const glm::vec3 xyz(
				0.4122214708f * lrgb.r + 0.5363325363f * lrgb.g + 0.0514459929f * lrgb.b,
				0.2119034982f * lrgb.r + 0.6806995451f * lrgb.g + 0.1073969566f * lrgb.b,
				0.0883024619f * lrgb.r + 0.2817188376f * lrgb.g + 0.6299787005f * lrgb.b
			);

			return glm::vec3(
				std::cbrt(0.2104542553f * xyz.x + 0.7936177850f * xyz.y - 0.0040720468f * xyz.z),
				std::cbrt(1.9779984951f * xyz.x - 2.4285922050f * xyz.y + 0.4505937099f * xyz.z),
				std::cbrt(0.0259040371f * xyz.x + 0.7827717662f * xyz.y - 0.8086757660f * xyz.z)
			);
		}

		inline glm::vec3 oklabToRGB(const glm::vec3& lab) noexcept {
			const float l = lab.x * lab.x * lab.x;
			const float m = lab.y * lab.y * lab.y;
			const float s = lab.z * lab.z * lab.z;

			const glm::vec3 xyz(
				1.2270138511f * l - 0.5577999806f * m + 0.2812561484f * s,
				-0.0405801784f * l + 1.1122568696f * m - 0.0716766787f * s,
				-0.0763812845f * l + 0.6353551987f * m + 0.4370302234f * s
			);

			const glm::vec3 lrgb(
				3.2409699419f * xyz.x - 1.5373831776f * xyz.y - 0.4986107603f * xyz.z,
				-0.9692436363f * xyz.x + 1.8759675015f * xyz.y + 0.0415550574f * xyz.z,
				0.0556300797f * xyz.x - 0.2039769589f * xyz.y + 1.0569715142f * xyz.z
			);

			return glm::clamp(glm::pow(lrgb, glm::vec3(1.0f / 2.4f)), 0.0f, 1.0f);
		}

		inline glm::vec3 rgbToHSV(const glm::vec3& rgb) noexcept {
			float cmax = std::max({ rgb.r, rgb.g, rgb.b });
			float cmin = std::min({ rgb.r, rgb.g, rgb.b });
			float diff = cmax - cmin;

			glm::vec3 hsv(0.0f);
			hsv.z = cmax; // Value

			if (cmax != 0.0f) {
				hsv.y = diff / cmax; // Saturation
			}

			if (diff != 0.0f) {
				if (cmax == rgb.r) {
					hsv.x = (rgb.g - rgb.b) / diff + (rgb.g < rgb.b ? 6.0f : 0.0f);
				} else if (cmax == rgb.g) {
					hsv.x = (rgb.b - rgb.r) / diff + 2.0f;
				} else {
					hsv.x = (rgb.r - rgb.g) / diff + 4.0f;
				}
				hsv.x /= 6.0f;
			}

			return hsv;
		}

		inline glm::vec3 hsvToRGB(const glm::vec3& hsv) noexcept {
			if (hsv.y == 0.0f) {
				return glm::vec3(hsv.z); // Gray
			}

			float h = hsv.x * 6.0f;
			int i = static_cast<int>(h);
			float f = h - static_cast<float>(i);
			float p = hsv.z * (1.0f - hsv.y);
			float q = hsv.z * (1.0f - hsv.y * f);
			float t = hsv.z * (1.0f - hsv.y * (1.0f - f));

			switch (i % 6) {
				case 0: return glm::vec3(hsv.z, t, p);
				case 1: return glm::vec3(q, hsv.z, p);
				case 2: return glm::vec3(p, hsv.z, t);
				case 3: return glm::vec3(p, q, hsv.z);
				case 4: return glm::vec3(t, p, hsv.z);
				default: return glm::vec3(hsv.z, p, q);
			}
		}
	}

	float Color::getLuminance() const noexcept {
		return 0.2126f * r + 0.7152f * g + 0.0722f * b;
	}

	float Color::getContrastRatio(const Color& c1, const Color& c2) noexcept {
		float l1 = c1.getLuminance();
		float l2 = c2.getLuminance();
		float lighter = std::max(l1, l2);
		float darker = std::min(l1, l2);
		return (lighter + 0.05f) / (darker + 0.05f);
	}

	// Random
	Color Color::random(float minLuminance, bool preferVibrant) noexcept {
		if (preferVibrant) {
			glm::vec3 hsv = ColorSpace::generateRandomHSV(0.7f, 0.5f);
			return fromHSV(hsv);
		} else {
			glm::vec3 lab = ColorSpace::generateRandomOklab(minLuminance);
			glm::vec3 rgb = ColorSpace::oklabToRGB(lab);
			return Color(rgb);
		}
	}

	Color Color::randomPastel(float minLuminance /*= 0.7f*/) noexcept {
		glm::vec3 lab = ColorSpace::generateRandomOklab(minLuminance);
		lab.y *= 0.3f;
		lab.z *= 0.3f;
		glm::vec3 rgb = ColorSpace::oklabToRGB(lab);
		return Color(rgb);
	}

	Color Color::randomPrimary() noexcept {
		static const std::array<Color, 8> primaryColors = {
			Color(1.0f, 0.0f, 0.0f),  // Red
			Color(0.0f, 1.0f, 0.0f),  // Green
			Color(0.0f, 0.0f, 1.0f),  // Blue
			Color(1.0f, 1.0f, 0.0f),  // Yellow
			Color(1.0f, 0.0f, 1.0f),  // Magenta
			Color(0.0f, 1.0f, 1.0f),  // Cyan
			Color(1.0f, 0.5f, 0.0f),  // Orange
			Color(0.5f, 0.0f, 1.0f)   // Purple
		};

		return primaryColors[StaticRandom::getIntInRange(0, (int32_t)(primaryColors.size() - 1))];
	}

	Color Color::randomWithContrast(const Color& baseColor, float minContrastRatio /*= 4.5f*/) noexcept {
		Color newColor;
		float contrast;
		int attempts = 0;
		const int maxAttempts = 50;

		do {
			newColor = random(0.0f, true);
			contrast = getContrastRatio(baseColor, newColor);
			attempts++;
		} while (contrast < minContrastRatio && attempts < maxAttempts);

		return newColor;
	}

	// Mixing
	Color Color::mix(const Color& col1, const Color& col2, float t) noexcept {
		t = std::clamp(t, 0.0f, 1.0f);
		glm::vec3 lab1 = ColorSpace::rgbToOklab(glm::vec3(col1.r, col1.g, col1.b));
		glm::vec3 lab2 = ColorSpace::rgbToOklab(glm::vec3(col2.r, col2.g, col2.b));
		glm::vec3 labMix = glm::mix(lab1, lab2, t);
		glm::vec3 rgbMix = ColorSpace::oklabToRGB(labMix);
		return Color(rgbMix, glm::mix(col1.a, col2.a, t));
	}

	Color Color::mixRGB(const Color& col1, const Color& col2, float t) noexcept {
		t = std::clamp(t, 0.0f, 1.0f);
		return Color(
			glm::mix(col1.r, col2.r, t),
			glm::mix(col1.g, col2.g, t),
			glm::mix(col1.b, col2.b, t),
			glm::mix(col1.a, col2.a, t)
		);
	}
}