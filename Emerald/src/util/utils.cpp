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
	bool LoadTexture(const String& path, bool flip, function<void(byte* data, uint width, uint height)> callback) {
		if (!FileSystem::DoesFileExist(path)) {
			LOG_ERROR("[~gTexture~x] file at ~1%s~x does not exist!", path.c_str());
			return false;
		}

		int bpc;
		int width, height;

		stbi_set_flip_vertically_on_load(flip);
		byte* data = stbi_load(path.c_str(), &width, &height, &bpc, 4);

		if (bpc != 3 && bpc != 4) {
			LOG_ERROR("[~gTexture~x] Unsupported image bit-depth (%d) ~1%s", bpc, path.c_str());
			stbi_image_free(data);
			return false;
		}

		if (data) {
			LOG("[~gTexture~x] Loaded ~1%s", path.c_str());
			callback(data, width, height);
			stbi_image_free(data);
			return true;
		} else LOG_ERROR("[~gTexture~x] Failed to load ~1%s", path.c_str());
		return false;
	}
}