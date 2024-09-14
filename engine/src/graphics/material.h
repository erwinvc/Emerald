#pragma once
#include "shaders/shader.h"
#include "../util/buffer.h"

namespace emerald {
	class Material : public RefCounted {
	public:
		Material(std::string name, const Ref<Shader>& shader);

		void initializeBuffer();
		void updateForRendering();

		template <typename T>
		void Set(const std::string& name, const T& value, uint32_t index = 0) {
			auto uniform = getShaderUniform(name);
			//ASSERT(uniform, "Could not find uniform");
			if (!uniform)
				return;

			m_uniformStorageBuffer.write((byte*)&value, uniform->m_size, uniform->m_offset + uniform->m_size * index);
		}

		template <typename T>
		void SetArray(const std::string& name, const T* value, uint32_t count) {
			auto uniform = getShaderUniform(name);
			//ASSERT(uniform, "Could not find uniform");
			if (!uniform)
				return;

			uint32_t totalSize = uniform->m_size * count;
			m_uniformStorageBuffer.write((byte*)&value, totalSize, uniform->m_offset);
		}

		template<typename T>
		T& Get(const std::string& name) {
			auto uniform = getShaderUniform(name);
			ASSERT(uniform, "Could not find uniform");
			return m_uniformStorageBuffer.read<T>(uniform->m_offset);
		}
		const ShaderUniform* getShaderUniform(const std::string& name);

	private:
		std::string m_name;
		Ref<Shader> m_shader;
		Buffer<byte> m_uniformStorageBuffer;
	};
}