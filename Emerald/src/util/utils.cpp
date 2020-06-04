#include "stdafx.h"

String_t Format_t(String_t fmt, ...) {
	static char vaBuffer[0x200];
	va_list ap;
	va_start(ap, fmt);
	vsprintf_s(vaBuffer, fmt, ap);
	va_end(ap);
	return vaBuffer;
}

String_t Format_b(char* buffer, uint32 size, String_t fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsprintf_s(buffer, size, fmt, ap);
	va_end(ap);
	return buffer;
}


String Format(String_t fmt, ...) {
	char vaBuffer[0x200];
	va_list ap;
	va_start(ap, fmt);
	vsprintf_s(vaBuffer, fmt, ap);
	va_end(ap);
	return String(vaBuffer);
}

namespace Utils {
	bool EndsWith(const std::string& str, const char* suffix, unsigned suffixLen) {
		return str.size() >= suffixLen && 0 == str.compare(str.size() - suffixLen, suffixLen, suffix, suffixLen);
	}

	bool EndsWith(const std::string& str, const char* suffix) {
		return EndsWith(str, suffix, std::string::traits_type::length(suffix));
	}

	bool StartsWith(const std::string& str, const char* prefix, unsigned prefixLen) {
		return str.size() >= prefixLen && 0 == str.compare(0, prefixLen, prefix, prefixLen);
	}

	bool StartsWith(const std::string& str, const char* prefix) {
		return StartsWith(str, prefix, std::string::traits_type::length(prefix));
	}

	void LTrim(String& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

	void RTrim(String& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	void Trim(String& s) {
		LTrim(s);
		RTrim(s);
	}

	vector<String> Split(const String& s, const String& splitter) {
		size_t pos;
		vector<String> out;
		String ss = s;
		while ((pos = ss.find(splitter)) != String::npos) {
			String token = ss.substr(0, pos);
			out.push_back(token);
			ss.erase(0, pos + splitter.length());
		}
		out.push_back(ss);
		return out;
	}

	String RotateString90(const String& toRotate, int x, int y) {
		int size = (x * y);

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

	String FlipStringHorizontal(const String& toFlip, int x, int y) {
		int size = x * y;
		String newStr;
		newStr.reserve(size);
		for (int yy = 0; yy < y; yy++) {
			for (int xx = 0; xx < x; xx++) {
				newStr[xx + (yy * y)] = toFlip[((x - 1) - xx) + (yy * y)];
			}
		}
		newStr[size] = 0x00;
		return newStr;
	}

	String FlipStringVertical(const String& toFlip, int x, int y) {
		int size = x * y;
		String newStr;
		newStr.reserve(size);
		for (int yy = 0; yy < y; yy++) {
			for (int xx = 0; xx < x; xx++) {
				newStr[xx + (yy * y)] = toFlip[xx + ((y - yy - 1) * y)];
			}
		}
		newStr[size] = 0x00;
		return newStr;
	}

	String ReplaceString(String subject, const String& search, const string& replace) {
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != String::npos) {
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
		return subject;
	}

	void DoTimedFunction(int* timer, int timeMS, function<void()> function) {
		if (*timer < (int)GetTickCount()) {
			*timer = (int)GetTickCount() + timeMS;
			function();
		}
	}

	void SetPositionInFrontOfCam(glm::vec3& dest, const Camera* cam, float distance) {
		const float x = cam->transform.position.x;
		const float y = cam->transform.position.y;
		const float z = cam->transform.position.z;

		dest.x = x + Math::Sin(cam->transform.rotation.y) * distance * Math::Abs(Math::Cos(cam->transform.rotation.x));
		dest.y = y - Math::Sin(cam->transform.rotation.x) * distance;
		dest.z = z - Math::Cos(cam->transform.rotation.y) * distance * Math::Abs(Math::Cos(cam->transform.rotation.x));
	}

	glm::vec3 RotationToDirection(glm::vec3 rotation) {
		glm::vec3 mirrorRot = rotation;
		mirrorRot.y = -mirrorRot.y; //Whatthefeck
		mirrorRot.x = -mirrorRot.x; //Honestly no clue why this is necessary. Cba to debug rn
		return glm::quat(mirrorRot) * glm::vec3(0, 0, -1);
	}
}

namespace GLUtils {
	String_t ShaderTypeToString(int type, bool upperCase) {
		switch (type) {
			case GL_VERTEX_SHADER: return upperCase ? "Vertex" : "vertex";
			case GL_GEOMETRY_SHADER: return upperCase ? "Geometry" : "geometry";
			case GL_FRAGMENT_SHADER: return upperCase ? "Fragment" : "fragment";
			case GL_TESS_EVALUATION_SHADER: return upperCase ? "Tessellation evaluation" : "tessellation evaluation";
			case GL_TESS_CONTROL_SHADER: return upperCase ? "Tessellation control" : "tessellation control";
		}
		return "NULL";
	}

	String_t GetFBOStatus(GLenum status) {
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

	String GLErrorToString(int error) {
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

	void EnableBlending() {
		GL(glEnable(GL_BLEND));
		GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GL(glDisable(GL_DEPTH_TEST));
	}

	void DisableBlending() {
		GL(glEnable(GL_DEPTH_TEST));
		GL(glDisable(GL_BLEND));
	}

	void EnableDepthTest() {
		GL(glEnable(GL_DEPTH_TEST));
	}

	void DisableDepthTest() {
		GL(glDisable(GL_DEPTH_TEST));
	}
}

namespace TextureUtils {
	bool LoadTexture(const Path& filePath, bool flip, function<void(const LoadedTexture& data)> callback, int goalChannels) {
		if (!FileSystem::DoesFileExist(filePath.GetFullPath())) {
			LOG_WARN("[~gTexture~x] ~1%s.png~x at ~1%s~x does not exist!", filePath.GetFileName().c_str(), filePath.GetDirectory().c_str());
			return false;
		}

		int channelCount;
		int width, height;

		stbi_set_flip_vertically_on_load(flip);
		byte* data = stbi_load(filePath.GetFullPath().c_str(), &width, &height, &channelCount, goalChannels);

		//if (bpc != 3 && bpc != 4) {
		//	LOG_ERROR("[~gTexture~x] Unsupported image bit-depth (%d) ~1%s", bpc, path.c_str());
		//	stbi_image_free(data);
		//	return false;
		//}

		//if (channelCount < 1 || channelCount > 4) LOG_ERROR("[~gTexture~x] Unsupported image bit-depth (%d) ~1%s", channelCount, filePath.GetFileName().c_str());

		int size = channelCount * width * height;

		if (data) {
			//LOG("[~gTexture~x] Loaded ~1%s", path.c_str());
			LoadedTexture textureData = { data, (uint)width, (uint)height, channelCount, size };
			callback(textureData);
			stbi_image_free(data);
			return true;
		} //else LOG_ERROR("[~gTexture~x] Failed to load ~1%s", path.c_str());
		return false;
	}
}