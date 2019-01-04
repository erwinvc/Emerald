#pragma once

template <typename T, size_t N>
constexpr size_t NUMOF(T(&)[N]) { return N; }

namespace Math {

    static const float PI = 3.1415927F;
    static const float HALF_PI = 1.5707964F;
    static const float THIRD_PI = 1.0471976F;
    static const float QUARTER_PI = 0.7853982F;


    inline float ToRadians(float degrees) {
        return (float)(degrees * (3.14 / 180.0f));
    }

    inline float ToDegrees(float radians) {
        return (float)(radians * (180.0f / 3.14));
    }

    inline float abs(float val) {
        return val <= 0.0F ? 0.0F - val : val;
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

    inline float RandomF(float min, float max) {
        float r = (float)rand() / (float)RAND_MAX;
        return r * (max - min) + min;
    }

    inline int RandomI(int min, int max) {
        return min + rand() % ((max + 1) - min);
    }


}