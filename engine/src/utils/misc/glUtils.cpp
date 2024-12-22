#include "eepch.h"
#include "graphics/textures/textureDesc.h"
#include "utils/math/color.h"
#include "graphics/shaders/shader.h"

namespace emerald {
	namespace GLUtils {
		const char* shaderTypeToString(int type, bool upperCase) {
			switch (type) {
				case GL_VERTEX_SHADER: return upperCase ? "Vertex" : "vertex";
				case GL_GEOMETRY_SHADER: return upperCase ? "Geometry" : "geometry";
				case GL_FRAGMENT_SHADER: return upperCase ? "Fragment" : "fragment";
				case GL_TESS_EVALUATION_SHADER: return upperCase ? "Tessellation evaluation" : "tessellation evaluation";
				case GL_TESS_CONTROL_SHADER: return upperCase ? "Tessellation control" : "tessellation control";
			}
			throw std::runtime_error("Unknown shader type");
		}

		const char* shaderUniformTypeToString(ShaderUniformType type, bool upperCase) {
			switch (type) {
				case ShaderUniformType::NONE: return upperCase ? "None" : "none";
				case ShaderUniformType::BOOL: return upperCase ? "Bool" : "bool";
				case ShaderUniformType::INT:  return upperCase ? "Int" : "int";
				case ShaderUniformType::UINT: return upperCase ? "UInt" : "uint";
				case ShaderUniformType::FLOAT:return upperCase ? "Float" : "float";
				case ShaderUniformType::VEC2: return upperCase ? "Vec2" : "vec2";
				case ShaderUniformType::VEC3: return upperCase ? "Vec3" : "vec3";
				case ShaderUniformType::VEC4: return upperCase ? "Vec4" : "vec4";
				case ShaderUniformType::MAT3: return upperCase ? "Mat3" : "mat3";
				case ShaderUniformType::MAT4: return upperCase ? "Mat4" : "mat4";

			}
			throw std::runtime_error("Unknown shader uniform type");
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

		ShaderUniformType glTypeToShaderUniformType(uint32_t type) {
			switch (type) {
				case GL_FLOAT: 				return ShaderUniformType::FLOAT;
				case GL_FLOAT_VEC2: 		return ShaderUniformType::VEC2;
				case GL_FLOAT_VEC3: 		return ShaderUniformType::VEC3;
				case GL_FLOAT_VEC4: 		return ShaderUniformType::VEC4;
				case GL_INT: 				return ShaderUniformType::INT;
				case GL_FLOAT_MAT4: 		return ShaderUniformType::MAT4;
				case GL_SAMPLER_CUBE: 		return ShaderUniformType::INT;
				case GL_SAMPLER_1D: 		return ShaderUniformType::INT;
				case GL_SAMPLER_2D: 		return ShaderUniformType::INT;
				case GL_SAMPLER_3D: 		return ShaderUniformType::INT;
				case GL_SAMPLER_2D_ARRAY:	return ShaderUniformType::INT;
			}
			Log::error("Unknow GL type {}", type);
			return ShaderUniformType();
		}

		uint32_t getUniformSize(ShaderUniformType type) {
			switch (type) {
				case ShaderUniformType::INT:   return sizeof(int);
				case ShaderUniformType::FLOAT: return sizeof(float);
				case ShaderUniformType::VEC2:  return sizeof(float) * 2;
				case ShaderUniformType::VEC3:  return sizeof(float) * 3;
				case ShaderUniformType::VEC4:  return sizeof(float) * 4;
				case ShaderUniformType::MAT4:  return sizeof(float) * 16;
			}
			Log::error("Unknow ShaderUniformType type {}", (uint32_t)type);
			return 0;
		};

		bool isDepthFormat(TextureFormat format) {
			switch (format) {
				case TextureFormat::DEPTH24STENCIL8:
				case TextureFormat::DEPTH32F:
					return true;
			}
			return false;
		}

		void glClearColor(Color color) {
			GL(::glClearColor(color.r, color.g, color.b, color.a));
		}
	}

}