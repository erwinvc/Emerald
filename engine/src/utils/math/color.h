#pragma once
#include "imgui/imgui.h"

namespace emerald {
	class Color;
	namespace ColorSpace {
		glm::vec3 generateRandomOklab(float minLuminance) noexcept;
		glm::vec3 generateRandomHSV(float minSaturation, float minValue) noexcept;
		glm::vec3 rgbToOklab(const glm::vec3& rgb) noexcept;
		glm::vec3 oklabToRGB(const glm::vec3& lab) noexcept;
		glm::vec3 rgbToHSV(const glm::vec3& rgb) noexcept;
		glm::vec3 hsvToRGB(const glm::vec3& hsv) noexcept;
	}

	class Color {
	public:
		using ComponentArray = std::array<float, 4>;

		union {
			struct {
				float r, g, b, a;
			};
			ComponentArray components;
		};

		constexpr Color() noexcept : components{ 0.0f, 0.0f, 0.0f, 1.0f } {}
		constexpr Color(float r, float g, float b, float a = 1.0f) noexcept : components{ r, g, b, a } {}
		constexpr Color(uint32_t rgba) noexcept : components{ ((rgba >> 24) & 0xFF) / 255.0f, ((rgba >> 16) & 0xFF) / 255.0f, ((rgba >> 8) & 0xFF) / 255.0f, (rgba & 0xFF) / 255.0f } {}

		explicit constexpr Color(const glm::vec3& rgb, float alpha = 1.0f) noexcept : components{ rgb.r, rgb.g, rgb.b, alpha } {}
		explicit constexpr Color(const glm::vec4& rgba) noexcept : components{ rgba.r, rgba.g, rgba.b, rgba.a } {}

		static constexpr Color white() noexcept { return Color(1.0f, 1.0f, 1.0f); }
		static constexpr Color black() noexcept { return Color(0.0f, 0.0f, 0.0f); }
		static constexpr Color gray(float value = 0.5f) noexcept { return Color(value, value, value); }
		static constexpr Color red() noexcept { return Color(1, 0, 0); }
		static constexpr Color green() noexcept { return Color(0, 1, 0); }
		static constexpr Color blue() noexcept { return Color(0, 0, 1); }
		static constexpr Color yellow() noexcept { return Color(1, 1, 0); }
		static constexpr Color magenta() noexcept { return Color(1, 0, 1); }
		static constexpr Color cyan() noexcept { return Color(0, 1, 1); }
		static constexpr Color transparent() noexcept { return Color(0, 0, 0, 0); }
		static constexpr Color normalMap() noexcept { return Color(0.5f, 0.5f, 1); }

		constexpr uint32_t toRGBA8() const noexcept {
			return ((uint32_t)(std::clamp(a, 0.0f, 1.0f) * 255.0f) << 24) |
				((uint32_t)(std::clamp(b, 0.0f, 1.0f) * 255.0f) << 16) |
				((uint32_t)(std::clamp(g, 0.0f, 1.0f) * 255.0f) << 8) |
				((uint32_t)(std::clamp(r, 0.0f, 1.0f) * 255.0f));
		}

		//static constexpr Color fromRGBA8(uint32_t rgba) noexcept {
		//	return Color{
		//		(float)((rgba >> 24) & 0xFF) / 255.0f,
		//		(float)((rgba >> 16) & 0xFF) / 255.0f,
		//		(float)((rgba >> 8) & 0xFF) / 255.0f,
		//		(float)(rgba & 0xFF) / 255.0f
		//	};
		//}

		//static constexpr Color fromRGBA8(int32_t rgba) noexcept {
		//	return Color{
		//		(float)((rgba >> 24) & 0xFF) / 255.0f,
		//		(float)((rgba >> 16) & 0xFF) / 255.0f,
		//		(float)((rgba >> 8) & 0xFF) / 255.0f,
		//		(float)(rgba & 0xFF) / 255.0f
		//	};
		//}

		glm::vec3 toHSV() const noexcept {
			return ColorSpace::rgbToHSV(glm::vec3(r, g, b));
		}

		static Color fromHSV(const glm::vec3& hsv, float alpha = 1.0f) noexcept {
			glm::vec3 rgb = ColorSpace::hsvToRGB(hsv);
			return Color(rgb, alpha);
		}

		constexpr Color& clamp() noexcept {
			r = std::clamp(r, 0.0f, 1.0f);
			g = std::clamp(g, 0.0f, 1.0f);
			b = std::clamp(b, 0.0f, 1.0f);
			a = std::clamp(a, 0.0f, 1.0f);
			return *this;
		}

		float getLuminance() const noexcept;
		static float getContrastRatio(const Color& c1, const Color& c2) noexcept;

		// Random
		static Color random(float minLuminance = 0.0f, bool preferVibrant = false) noexcept;
		static Color randomPastel(float minLuminance = 0.7f) noexcept;
		static Color randomPrimary() noexcept;
		static Color randomWithContrast(const Color& baseColor, float minContrastRatio = 4.5f) noexcept;

		// Color mixing
		static Color mix(const Color& col1, const Color& col2, float t) noexcept;
		static Color mixRGB(const Color& col1, const Color& col2, float t) noexcept;

		constexpr Color operator*(float scalar) const noexcept {
			return Color(r * scalar, g * scalar, b * scalar, a * scalar);
		}

		constexpr Color& operator*=(float scalar) noexcept {
			r *= scalar;
			g *= scalar;
			b *= scalar;
			a *= scalar;
			return *this;
		}

		constexpr Color operator+(const Color& other) const noexcept {
			return Color(r + other.r, g + other.g, b + other.b, a + other.a);
		}

		constexpr Color& operator+=(const Color& other) noexcept {
			r += other.r;
			g += other.g;
			b += other.b;
			a += other.a;
			return *this;
		}

		// Conversion operators
		constexpr operator ImVec4() const noexcept { return ImVec4(r, g, b, a); }
		constexpr operator glm::vec4() const noexcept { return glm::vec4(r, g, b, a); }
		constexpr operator glm::vec3() const noexcept { return glm::vec3(r, g, b); }
	};

	constexpr Color operator*(float scalar, const Color& color) noexcept {
		return color * scalar;
	}
}