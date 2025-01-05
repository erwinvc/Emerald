#pragma once
#include "graphics/shaders/shader.h"
#include "utils/datastructures/buffer.h"
#include "../textures/texture.h"

namespace emerald {
	class Material : public RefCounted {
	public:
		struct TextureBinding {
			Ref<Texture> texture = nullptr;
			int32_t unit = 0;
		};

		Material(std::string name, const Ref<Shader>& shader);
		virtual ~Material() = default;

		const std::string& getName() { return m_name; }
		void initializeBuffer();
		void updateForRendering();

		template <typename T>
		void set(const std::string& name, const T& value, uint32_t index = 0) {
			static_assert(!std::is_pointer_v<T>, "Cannot pass pointers to uniform setter");

			auto uniform = getShaderUniform(name);
			//ASSERT(uniform, "Could not find uniform");
			if (!uniform)
				return;

			m_uniformStorageBuffer.write((byte*)&value, uniform->m_size, uniform->m_offset + uniform->m_size * index);
		}

		template <typename T>
		void setArray(const std::string& name, const T* value, uint32_t count) {
			auto uniform = getShaderUniform(name);
			//ASSERT(uniform, "Could not find uniform");
			if (!uniform)
				return;

			uint32_t totalSize = uniform->m_size * count;
			m_uniformStorageBuffer.write((byte*)&value, totalSize, uniform->m_offset);
		}

		template <typename T>
		T get(const std::string& name) {
			auto uniform = getShaderUniform(name);
			ASSERT(uniform, "Could not find uniform");
			return m_uniformStorageBuffer.read<T>(uniform->m_offset);
		}

		const ShaderUniform* getShaderUniform(const std::string& name);
		std::mutex mutex;
		void setTexture(const std::string& name, const Ref<Texture>& texture) {
			if (!texture) {
				auto it = m_textureBindings.find(name);
				if (it != m_textureBindings.end()) {
					it->second.texture = nullptr;
				}
			} else {
				auto uniform = getShaderUniform(name);
				//ASSERT(uniform, "Could not find uniform");
				if (!uniform)
					return;

				if (m_textureBindings.find(name) == m_textureBindings.end()) {
					m_textureBindings[name] = TextureBinding{ texture, (int32_t)m_textureBindings.size() };
					set(name, m_textureBindings[name].unit);
				} else {
					m_textureBindings[name].texture = texture;
				}
			}
		}

		Ref<Texture> getTexture(const std::string& name) const;

	private:
		std::string m_name;
		Ref<Shader> m_shader;
		Buffer<byte> m_uniformStorageBuffer;
		std::unordered_map<std::string, TextureBinding> m_textureBindings;
	};
}