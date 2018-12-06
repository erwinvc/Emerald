#pragma once

template <typename T, size_t N>
constexpr size_t NUMOF(T(&)[N]) { return N; }

namespace Math {

	inline float toRadians(float degrees) {
		return (float)(degrees * (3.14 / 180.0f));
	}

	inline float toDegrees(float radians) {
		return (float)(radians * (180.0f / 3.14));
	}

	inline int sign(float value) {
		return (value > 0) - (value < 0);
	}

	inline float sin(float angle) {
		return (float)::sin(angle);
	}

	inline float cos(float angle) {
		return (float)::cos(angle);
	}

	inline float tan(float angle) {
		return (float)::tan(angle);
	}

	inline float sqrt(float value) {
		return (float)::sqrt(value);
	}

	inline float rsqrt(float value) {
		return 1.0f / (float)::sqrt(value);
	}

	inline float asin(float value) {
		return (float)::asin(value);
	}

	inline float acos(float value) {
		return (float)::acos(value);
	}

	inline float atan(float value) {
		return (float)::atan(value);
	}

	inline float atan2(float y, float x) {
		return (float)::atan2(y, x);
	}

	inline float _min(float value, float minimum) {
		return (value < minimum) ? minimum : value;
	}

	inline float _max(float value, float maximum) {
		return (value > maximum) ? maximum : value;
	}

	inline float clamp(float value, float minimum, float maximum) {
		return (value > minimum) ? (value < maximum) ? value : maximum : minimum;
	}

	inline float randomf(float min, float max) {
		float r = (float) rand() / (float)RAND_MAX;
		return r * (max - min) + min;
	}

	inline int randomi(int min, int max)
	{
		return min + rand() % ((max + 1) - min);
	}
}