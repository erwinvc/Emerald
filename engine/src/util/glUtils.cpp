#include "eepch.h"
#include "graphics/textureDesc.h"
#include "color.h"

namespace emerald {
	namespace GLUtils {
		void GLAPIENTRY debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

			Log::info("GL CALLBACK: type = 0x{0:x}, severity = 0x{1:x}, message = {2}", type, severity, message);
		}

		void setDebugMessageCallback() {
			glDebugMessageCallback(debugMessageCallback, 0);
		}

		const char* shaderTypeToString(int type, bool upperCase) {
			switch (type) {
				case GL_VERTEX_SHADER: return upperCase ? "Vertex" : "vertex";
				case GL_GEOMETRY_SHADER: return upperCase ? "Geometry" : "geometry";
				case GL_FRAGMENT_SHADER: return upperCase ? "Fragment" : "fragment";
				case GL_TESS_EVALUATION_SHADER: return upperCase ? "Tessellation evaluation" : "tessellation evaluation";
				case GL_TESS_CONTROL_SHADER: return upperCase ? "Tessellation control" : "tessellation control";
			}
			return "NULL";
		}

		const char* getFBOStatus(GLenum status) {
			switch (status) {
				case GL_FRAMEBUFFER_COMPLETE:						return "no error";
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:			return "incomplete attachment";
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	return "missing Attachment";
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:			return "draw Buffer";
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:			return "read Buffer";
				case GL_FRAMEBUFFER_UNSUPPORTED:					return "snsupported Framebuffer configuration";
					//case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:			return "dimensions do not match";
					//case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:				return "formats error";
				default:												return "unkown Framebuffer Object error";
			}
		}

		const char* glErrorToString(int error) {
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

		bool isDepthFormat(TextureFormat format) {
			switch (format) {
				case TextureFormat::DEPTH24STENCIL8:
				case TextureFormat::DEPTH32F:
					return true;
			}
			return false;
		}

		void glClearColor(Color color) {
			::glClearColor(color.r, color.g, color.b, color.a);
		}
	}

}