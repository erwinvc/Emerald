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
	void setPositionInFrontOfCam(Vector3& dest, const Camera* cam, float distance) {
		const float x = cam->m_position.x;
		const float y = cam->m_position.y;
		const float z = cam->m_position.z;

		dest.x = x + Math::Sin(cam->m_rotation.yaw) * distance * Math::Abs(Math::Cos(cam->m_rotation.pitch));
		dest.y = y - Math::Sin(cam->m_rotation.pitch) * distance;
		dest.z = z - Math::Cos(cam->m_rotation.yaw) * distance * Math::Abs(Math::Cos(cam->m_rotation.pitch));
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
}

namespace TextureUtils {
	bool LoadTexture(const String& path, bool flip, function<void(const LoadedTexture& data)> callback) {
		if (!FileSystem::DoesFileExist(path)) {
			LOG_WARN("[~gTexture~x] file at ~1%s~x does not exist!", path.c_str());
			return false;
		}

		int channelCount;
		int width, height;

		stbi_set_flip_vertically_on_load(flip);
		byte* data = stbi_load(path.c_str(), &width, &height, &channelCount, 4);

		//if (bpc != 3 && bpc != 4) {
		//	LOG_ERROR("[~gTexture~x] Unsupported image bit-depth (%d) ~1%s", bpc, path.c_str());
		//	stbi_image_free(data);
		//	return false;
		//}

		if (channelCount < 1 || channelCount > 4) LOG_ERROR("[~gTexture~x] Unsupported image bit-depth (%d) ~1%s", channelCount, path.c_str());

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