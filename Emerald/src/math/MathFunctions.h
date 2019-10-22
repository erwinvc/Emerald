#pragma once

template <typename T, size_t N>
constexpr size_t NUMOF(T(&)[N]) { return N; }

namespace Math {
	static const float PI = 3.1415927f;
	static const float TWO_PI = 6.2831854f;
	static const float HALF_PI = 1.5707964f;
	static const float THIRD_PI = 1.0471976f;
	static const float QUARTER_PI = 0.7853982f;

	static const float MIN_FLOAT = FLT_MIN;
	static const float MAX_FLOAT = FLT_MAX;
	static const int MIN_INT = INT_MIN;
	static const int MAX_INT = INT_MAX;
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

	inline float Min(float value, float value2) {
		return (value < value2) ? value : value2;
	}

	inline int Min(int value, int value2) {
		return (value < value2) ? value : value2;
	}

	inline int Max(int value, int value2) {
		return (value > value2) ? value : value2;
	}

	inline float Max(float value, float value2) {
		return (value > value2) ? value : value2;
	}

	inline float Sign(float value, float amount = 1.0f) {
		if (value == 0) return 0;
		return value < 0 ? -amount : amount;
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

	template<typename T>
	inline bool Within(T valMin, T valMax, T min, T max) {
		return valMin < max && valMax > min;
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

	inline Vector3 PointOnUnitSphere(float u, float v) {
		float theta = u * 2.0f * Math::PI;
		float phi = Math::Acos(2.0f * v - 1.0f);
		float sinTheta = Math::Sin(theta);
		float cosTheta = Math::Cos(theta);
		float sinPhi = Math::Sin(phi);
		float cosPhi = Math::Cos(phi);
		float x = sinPhi * cosTheta;
		float y = sinPhi * sinTheta;
		float z = cosPhi;
		return { x, y, z };
	}

	inline Vector3 RandomOnUnitSphere() {
		float u = Math::RandomFloat(0.0f, 1.0f);
		float v = Math::RandomFloat(0.0f, 1.0f);
		return PointOnUnitSphere(u, v);
	}


	inline float Lerp(float toEase, float easeFrom, float percent) {
		return (toEase + percent * (easeFrom - toEase));
	}

	inline float Ease(float value, float easeTo, float multiplier) {
		return value + (value < easeTo ? Abs(value - easeTo) / multiplier : -Abs(value - easeTo) / multiplier);
	}

	inline float Round(float val) {
		return ::round(val);
	}

	inline float Floor(float val) {
		return ::floor(val);
	}

	inline float Map(float value, float minFrom, float maxFrom, float minTo, float maxTo) {
		return minTo + (maxTo - minTo) * ((value - minFrom) / (maxFrom - minFrom));
	}
}
