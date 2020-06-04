#pragma once

class Camera;
class Path;

#define ASSERT(x, ...) \
		if (!(x)) {\
			LOG("~r*************************"); \
			LOG("~r    ASSERTION FAILED!    "); \
			LOG("~r*************************"); \
			LOG("~r%s:%d", __FILE__, __LINE__); \
			LOG("~rCondition: %s", #x); \
			LOG_ERROR(__VA_ARGS__); \
			__debugbreak(); \
}

String_t Format_t(String_t fmt, ...);
String_t Format_b(char* buffer, uint32 size, String_t fmt, ...);
String Format(String_t fmt, ...);

namespace Utils {
	static void nullfunc() {}

	//String
	bool EndsWith(const std::string& str, const char* suffix, unsigned suffixLen);
	bool EndsWith(const std::string& str, const char* suffix);
	bool StartsWith(const std::string& str, const char* prefix, unsigned prefixLen);
	bool StartsWith(const std::string& str, const char* prefix);

	void LTrim(String& s);

	void RTrim(String& s);
	void Trim(String& s);

	vector<String> Split(const String& s, const String& splitter);

	String RotateString90(const String& toRotate, int x, int y);

	String FlipStringHorizontal(const String& toRotate, int x, int y);
	String FlipStringVertical(const String& toRotate, int x, int y);


	String ReplaceString(String subject, const String& search, const string& replace);

	void DoTimedFunction(int* timer, int timeMS, function<void()> function);

	template<typename T>
	void RemoveFromVector(vector<T>& vec, const T& obj) {
		vec.erase(remove(vec.begin(), vec.end(), obj), vec.end());
	}

	template<typename T>
	bool VectorContains(vector<T>& vec, const T& obj) {
		return find(vec.begin(), vec.end(), obj) != vec.end();
	}

	//Math?
	void SetPositionInFrontOfCam(glm::vec3& dest, const Camera* cam, float distance);
	glm::vec3 RotationToDirection(glm::vec3 rotation);
}

namespace GLUtils {
	String_t ShaderTypeToString(int type, bool upperCase = false);
	String_t GetFBOStatus(GLenum status);
	String GLErrorToString(int error);

	void EnableBlending();
	void DisableBlending();

	void EnableDepthTest();
	void DisableDepthTest();
}

struct LoadedTexture {
	byte* m_data;
	uint m_width;
	uint m_height;
	int m_channelCount;
	int m_size;
};

namespace TextureUtils {
	bool LoadTexture(const Path& filePath, bool flip, function<void(const LoadedTexture& data)> callback, int goalChannels = 0);
}