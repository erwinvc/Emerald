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
	static vector<String> Split(String& s, const String& splitter) {
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

	static String RotateString90(const String& toRotate, int x, int y) {
		int size = (x*y);

		String* strs = new String[y];
		for (int i = 0; i < size; i++) {
			strs[i / y].push_back(toRotate[i]);
		}

		String rotated;
		for (size_t i = 0; i < size; i++) {
			String newRow;
			for (int j = y - 1; j >= 0; j--) {
				newRow.push_back(strs[j][i]);
			}
			rotated.append(newRow);
		}

		delete[] strs;
		return rotated;
	}

	String FlipStringHorizontal(const String& toRotate, int x, int y);
	String FlipStringVertical(const String& toRotate, int x, int y);


	static String ReplaceString(String subject, const String& search,
		const string& replace) {
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != String::npos) {
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
	static bool VectorContains(vector<T>& vec, const T& obj) {
		return find(vec.begin(), vec.end(), obj) != vec.end();
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
		case GL_TESS_EVALUATION_SHADER: return upperCase ? "Tessellation evaluation" : "tessellation evaluation";
		case GL_TESS_CONTROL_SHADER: return upperCase ? "Tessellation control" : "tessellation control";
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

	static String GLErrorToString(int error) {
		switch (error) {
		case 0x500: return "GL_INVALID_ENUM";
		case 0x501: return "GL_INVALID_VALUE";
		case 0x502: return "GL_INVALID_OPERATION";
		case 0x503: return "GL_STACK_OVERFLOW";
		case 0x504: return "GL_STACK_UNDERFLOW";
		case 0x505: return "GL_OUT_OF_MEMORY";
		case 0x506: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case 0x507: return "GL_CONTEXT_LOST";
		case 0x5031: return "GL_TABLE_TOO_LARGE1";
		}
		return "";
	}
}

struct LoadedTexture {
	byte* m_data;
	uint m_width;
	uint m_height;
	int m_channelCount;
	int m_size;
};

namespace TextureUtils {
	bool LoadTexture(const String& path, bool flip, function<void(const LoadedTexture& data)> callback);
}