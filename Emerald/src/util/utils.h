#pragma once

class Camera;

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
String Format(String_t fmt, ...);

namespace Utils {
	static void nullfunc() {}

	//String
	static vector<String> Split(String& s, String splitter) {
		size_t pos;
		vector<String> out;
		while ((pos = s.find(splitter)) != String::npos) {
			String token = s.substr(0, pos);
			out.push_back(token);
			s.erase(0, pos + splitter.length());
		}
		out.push_back(s);
		return out;
	}

	static String_t GetShortFilename(const char* filename) {
		const char* lastSlash = strrchr(filename, '/');
		if (lastSlash == nullptr) {
			lastSlash = strrchr(filename, '\\');
		}
		String_t shortFilename = lastSlash != nullptr ? lastSlash + 1 : filename;
		return shortFilename;
	}

	static string ReplaceString(string subject, const string& search,
		const string& replace) {
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != string::npos) {
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
		return subject;
	}

	static void DoTimedFunction(int* timer, int timeMS, function<void()> function) {
		if (*timer < (int)GetTickCount()) {
			*timer = (int)GetTickCount() + timeMS;
			function();
		}
	}

	template<typename T>
	static void RemoveFromVector(vector<T>& vec, const T& obj) {
		vec.erase(remove(vec.begin(), vec.end(), obj), vec.end());
	}

	template<typename T>
	static void VectorContains(vector<T>& vec, const T& obj) {
		return vec.find(obj) != vec.end();
	}

	//Math?
	void setPositionInFrontOfCam(Vector3& dest, const Camera* cam, float distance);
}

namespace GLUtils {
	static String_t ShaderTypeToString(int type, bool upperCase = false) {
		switch (type) {
		case GL_VERTEX_SHADER: return upperCase ? "Vertex" : "vertex";
		case GL_GEOMETRY_SHADER: return upperCase ? "Geometry" : "geometry";
		case GL_FRAGMENT_SHADER: return upperCase ? "Fragment" : "fragment";
		}
		return "NULL";
	}

	static String_t GetFBOStatus(GLenum status) {
		switch (status) {
		case GL_FRAMEBUFFER_COMPLETE_EXT:						return "no error";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:			return "incomplete attachment";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:	return "missing Attachment";
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:			return "dimensions do not match";
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:				return "formats error";
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:			return "draw Buffer";
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:			return "read Buffer";
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:					return "snsupported Framebuffer configuration";
		default:												return "unkown Framebuffer Object error";
		}
	}
}

namespace ImGui {
	static void Tooltip(String_t tooltip) {
		if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.5f) {
			ImGui::SetTooltip(tooltip);
		}
	}
}

namespace GLUtils {
	void EnableBlending();
	void DisableBlending();
}