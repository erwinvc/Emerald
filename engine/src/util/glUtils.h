#pragma once

namespace emerald {
	class Color;
	enum TextureFormat : int;

	namespace GLUtils {
		const char* shaderTypeToString(int type, bool upperCase = false);
		const char* getFBOStatus(GLenum status);
		const char* glErrorToString(int error);

		bool isDepthFormat(TextureFormat format);
		void glClearColor(Color color);
	}
}