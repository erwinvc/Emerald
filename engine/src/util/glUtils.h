#pragma once

namespace emerald {
	class Color;
	enum TextureFormat : int;

	namespace GLUtils {
		void GLAPIENTRY debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
		void setDebugMessageCallback();

		const char* shaderTypeToString(int type, bool upperCase = false);
		const char* getFBOStatus(GLenum status);
		const char* glErrorToString(int error);

		bool isDepthFormat(TextureFormat format);
		void glClearColor(Color color);
	}
}