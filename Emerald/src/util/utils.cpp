#include "stdafx.h"

String_t Format_t(String_t fmt, ...) {
	static char vaBuffer[0x200];
	va_list ap;
	va_start(ap, fmt);
	vsprintf_s(vaBuffer, fmt, ap);
	va_end(ap);
	return vaBuffer;
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
	void setPositionInFrontOfCam(glm::vec3& dest, const Camera* cam, float distance) {
		const float x = cam->transform.m_position.x;
		const float y = cam->transform.m_position.y;
		const float z = cam->transform.m_position.z;

		dest.x = x + Math::Sin(cam->transform.m_rotation.y) * distance * Math::Abs(Math::Cos(cam->transform.m_rotation.x));
		dest.y = y - Math::Sin(cam->transform.m_rotation.x) * distance;
		dest.z = z - Math::Cos(cam->transform.m_rotation.y) * distance * Math::Abs(Math::Cos(cam->transform.m_rotation.x));
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
}

namespace GLUtils {
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
	bool LoadTexture(const Path& filePath, bool flip, function<void(const LoadedTexture & data)> callback) {
		if (!FileSystem::DoesFileExist(filePath.GetFullPath())) {
			LOG_WARN("[~gTexture~x] ~1%s~x.png at ~1%s~x does not exist!", filePath.GetFileName().c_str(), filePath.GetDirectory().c_str());
			return false;
		}

		int channelCount;
		int width, height;

		stbi_set_flip_vertically_on_load(flip);
		byte* data = stbi_load(filePath.GetFullPath().c_str(), &width, &height, &channelCount, 0);

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