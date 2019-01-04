#pragma once

class Camera;

#define ASSERT(x, ...) \
		if (!(x)) {\
			LOG_ERROR("*************************"); \
			LOG_ERROR("    ASSERTION FAILED!    "); \
			LOG_ERROR("*************************"); \
			LOG_ERROR(__FILE__, ": ", __LINE__); \
			LOG_ERROR("Condition: ", #x); \
			LOG_ERROR(__VA_ARGS__); \
			__debugbreak(); \
}

String va(String_t fmt, ...);

namespace Utils {
    static String ReadFile(String path) {
        ifstream stream(path);
        string str((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
        stream.close();
        return str;
    }

    void setPositionInFrontOfCam(Vector3& dest, const Camera& cam, float distance);
}