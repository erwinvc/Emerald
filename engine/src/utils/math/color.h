#pragma once
#include "imgui/imgui.h"

namespace emerald {
	inline glm::vec3 rgbToOklab(float r, float g, float b) {
		// Convert from linear sRGB to XYZ
		glm::vec3 rgb(r, g, b);
		glm::vec3 lrgb = glm::pow(rgb, glm::vec3(2.4f));

		glm::vec3 xyz = glm::vec3(
			0.4122214708f * lrgb.r + 0.5363325363f * lrgb.g + 0.0514459929f * lrgb.b,
			0.2119034982f * lrgb.r + 0.6806995451f * lrgb.g + 0.1073969566f * lrgb.b,
			0.0883024619f * lrgb.r + 0.2817188376f * lrgb.g + 0.6299787005f * lrgb.b
		);

		// Convert from XYZ to Oklab
		float l = std::cbrt(xyz.x * 0.2104542553f + xyz.y * 0.7936177850f + xyz.z * -0.0040720468f);
		float m = std::cbrt(xyz.x * 1.9779984951f + xyz.y * -2.4285922050f + xyz.z * 0.4505937099f);
		float s = std::cbrt(xyz.x * 0.0259040371f + xyz.y * 0.7827717662f + xyz.z * -0.8086757660f);

		return glm::vec3(l, m, s);
	}

	inline glm::vec3 oklabToRGB(float l, float m, float s) {
		// Convert from Oklab to XYZ
		float l_ = l * l * l;
		float m_ = m * m * m;
		float s_ = s * s * s;

		glm::vec3 xyz(
			1.2270138511f * l_ + -0.5577999806f * m_ + 0.2812561484f * s_,
			-0.0405801784f * l_ + 1.1122568696f * m_ + -0.0716766787f * s_,
			-0.0763812845f * l_ + 0.6353551987f * m_ + 0.4370302234f * s_
		);

		// Convert from XYZ to linear sRGB
		glm::vec3 lrgb = glm::vec3(
			3.2409699419f * xyz.x - 1.5373831776f * xyz.y - 0.4986107603f * xyz.z,
			-0.9692436363f * xyz.x + 1.8759675015f * xyz.y + 0.0415550574f * xyz.z,
			0.0556300797f * xyz.x - 0.2039769589f * xyz.y + 1.0569715142f * xyz.z
		);

		// Apply gamma correction (approximate)
		return glm::pow(glm::clamp(lrgb, 0.0f, 1.0f), glm::vec3(1.0f / 2.4f));
	}

	class Color {
	public:
		static Color white() { return Color(1.0f, 1.0f, 1.0f); }
		static Color black() { return Color(0.0f, 0.0f, 0.0f); }
		static Color gray() { return Color(0.5f, 0.5f, 0.5f); }
		static Color silver() { return Color(0.75f, 0.75f, 0.75f); }

		static Color red() { return Color(1, 0, 0); }
		static Color green() { return Color(0, 1, 0); }
		static Color blue() { return Color(0, 0, 1); }

		static Color yellow() { return Color(1, 1, 0); }
		static Color magenta() { return Color(1, 0, 1); }
		static Color cyan() { return Color(0, 1, 1); }

		static Color transparent() { return Color(0, 0, 0, 0); }
		static Color normalMap() { return Color(0.5f, 0.5f, 1); }

		union {
			struct {
				float r;
				float g;
				float b;
				float a;
			};
			float values[4];
		};

		Color() : r(0), g(0), b(0), a(0) {}

		Color(int32_t hexValue) {
			r = ((hexValue >> 24) & 0xFF) / 255.0f;
			g = ((hexValue >> 16) & 0xFF) / 255.0f;
			b = ((hexValue >> 8) & 0xFF) / 255.0f;
			a = (hexValue & 0xFF) / 255.0f;
		}

		Color(uint32_t hexValue) {
			r = ((hexValue >> 24) & 0xFF) / 255.0f;
			g = ((hexValue >> 16) & 0xFF) / 255.0f;
			b = ((hexValue >> 8) & 0xFF) / 255.0f;
			a = (hexValue & 0xFF) / 255.0f;
		}

		Color(float val) : r(val), g(val), b(val), a(val) {}
		Color(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}

		void clamp();
		float getRGBCombined();

		operator ImVec4() const {
			return ImVec4(r, g, b, a);
		}

		friend Color operator*(const Color& color, float value) {
			return Color(color.r * value, color.g * value, color.b * value, color.a * value);
		}

		friend Color operator+(const Color& col1, const Color& col2) {
			return Color(col1.r + col2.r, col1.g + col2.g, col1.b + col2.b, col1.a + col2.a);
		}

		friend Color operator-(const Color& col1, const Color& col2) {
			return Color(col1.r - col2.r, col1.g - col2.g, col1.b - col2.b, col1.a - col2.a);
		}

		static Color random(float min = 0);
		static Color randomPrimary();

		static Color mixSimple(const Color& col1, const Color& col2, float val);
		static Color mix(const Color& col1, const Color& col2, float val);
	};
}