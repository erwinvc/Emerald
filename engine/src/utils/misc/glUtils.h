#pragma once
#include "graphics/shaders/shader.h"

namespace emerald {
	class Color;
	enum TextureFormat : int;

	namespace GLUtils {
		const char* shaderTypeToString(int type, bool upperCase = false);
		const char* getFBOStatus(GLenum status);
		const char* glErrorToString(int error);
		ShaderUniformType glTypeToShaderUniformType(uint32_t type);
		uint32_t getUniformSize(ShaderUniformType type);

		bool isDepthFormat(TextureFormat format);
		void glClearColor(Color color);
	}
}