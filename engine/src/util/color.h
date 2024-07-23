#pragma once
#include "stdincl.h"
#include "imgui.h"

namespace emerald {
	class Color8 {
	public:
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Color8() : r(0), g(0), b(0), a(0) {}
		Color8(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

		operator uint8_t* () const { return (uint8_t*)this; }
	};

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
			if ((hexValue & 0xFF000000) == 0) { // 6-digit hex
				r = ((hexValue >> 16) & 0xFF) / 255.0f;
				g = ((hexValue >> 8) & 0xFF) / 255.0f;
				b = (hexValue & 0xFF) / 255.0f;
				a = 1.0f;
			} else { // 8-digit hex
				r = ((hexValue >> 24) & 0xFF) / 255.0f;
				g = ((hexValue >> 16) & 0xFF) / 255.0f;
				b = ((hexValue >> 8) & 0xFF) / 255.0f;
				a = (hexValue & 0xFF) / 255.0f;
			}
		}

		Color(float val) : r(val), g(val), b(val), a(val) {}
		Color(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}

		Color8 toColor8();
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

		static Color mix(const Color& col1, const Color& col2, float val);
	};
}