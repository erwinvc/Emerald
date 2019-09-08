#pragma once

template <typename T, size_t N>
constexpr size_t NUMOF(T(&)[N]) { return N; }

namespace Math {
	static const float PI = 3.1415927f;
	static const float TWO_PI = 6.2831854f;
	static const float HALF_PI = 1.5707964f;
	static const float THIRD_PI = 1.0471976f;
	static const float QUARTER_PI = 0.7853982f;


	inline float ToRadians(float degrees) {
		return (float)(degrees * (3.14 / 180.0f));
	}

	inline float ToDegrees(float radians) {
		return (float)(radians * (180.0f / 3.14f));
	}

	inline int Abs(int val) {
		return val <= 0 ? 1 - val : val;
	}

	inline float Abs(float val) {
		return val <= 0.0f ? 0.0f - val : val;
	}

	inline float Sin(float angle) {
		return (float)::sin(angle);
	}

	inline float Cos(float angle) {
		return (float)::cos(angle);
	}

	inline float Tan(float angle) {
		return (float)::tan(angle);
	}

	inline float Sqrt(float value) {
		return (float)::sqrt(value);
	}

	inline float Rsqrt(float value) {
		return 1.0f / (float)::sqrt(value);
	}

	inline float Pow(float value, float pow) {
		return (float)::pow(value, pow);
	}

	inline float Asin(float value) {
		return (float)::asin(value);
	}

	inline float Acos(float value) {
		return (float)::acos(value);
	}

	inline float Atan(float value) {
		return (float)::atan(value);
	}

	inline float Atan2(float y, float x) {
		return (float)::atan2(y, x);
	}

	inline float Min(float value, float minimum) {
		return (value < minimum) ? minimum : value;
	}

	inline int Min(int value, int minimum) {
		return (value < minimum) ? minimum : value;
	}

	inline int Max(int value, int maximum) {
		return (value > maximum) ? maximum : value;
	}

	inline float Max(float value, float maximum) {
		return (value > maximum) ? maximum : value;
	}

	template<typename T>
	inline T Clamp(T val, T min, T max) {
		return val < min ? min : val > max ? max : val;
	}

	template<typename T>
	inline void Clamp(T *val, T min, T max) {
		*val = *val < min ? min : *val > max ? max : *val;
	}

	template<typename T>
	inline bool Within(T val, T min, T max) {
		return val <= max && val >= min;
	}

	inline float RandomFloat(float min, float max) {
		float r = (float)rand() / (float)RAND_MAX;
		return r * (max - min) + min;
	}

	inline float RandomFloat(float max) {
		float r = (float)rand() / (float)RAND_MAX;
		return r * max;
	}

	/// <summary> Generates a random integer </summary>
	/// <param name="min"> Inclusive minimum </param>
	/// <param name="max"> Inclusive maxmimum </param>
	inline int RandomInt(int min, int max) {
		return min + rand() % ((max + 1) - min);
	}

	inline float Lerp(float toEase, float easeFrom, float percent) {
		return (toEase + percent * (easeFrom - toEase));
	}

	inline float Ease(float value, float easeTo, float multiplier) {
		return value + (value < easeTo ? Abs(value - easeTo) / multiplier : -Abs(value - easeTo) / multiplier);
	}

	inline int Round(float val) {
		return (int)::round(val);
	}

	inline int Floor(float val) {
		return (int)::floor(val);
	}
}
