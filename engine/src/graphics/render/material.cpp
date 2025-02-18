#include "eepch.h"
#include "material.h"
#include "graphics/shaders/shader.h"
#include "imguiProfiler/Profiler.h"

namespace emerald {
	Material::Material(std::string name, const Ref<Shader>& shader)
		: m_name(name), m_shader(shader) {

		initializeBuffer();
	}

	void Material::initializeBuffer() {
		ASSERT(m_shader->isLoaded(), "Trying to create a material for a shader that is not yet loaded");
		const auto& uniformBuffers = m_shader->getUniformBuffers();
		uint32_t size = 0;

		for (auto& [name, buffer] : uniformBuffers) {
			size += buffer.m_size * buffer.m_count;
		}

		m_uniformStorageBuffer.reserve(size);
		m_uniformStorageBuffer.zeroInitialize();
	}

	void Material::updateForRendering() {
		m_shader->bind();

		int textureSlot = 0;

		for (const auto& [name, binding] : m_textureBindings) {
			if (binding.texture) {
				binding.texture->bind(binding.unit);
			}
		}

		PROFILE_BEGIN("Material update for rendering");
		const auto& uniformBuffers = m_shader->getUniformBuffers();
		PROFILE_GPU_BEGIN("Material update for rendering");
		for (auto& [name, uniform] : uniformBuffers) {
			uint32_t count = uniform.m_isArray ? uniform.m_count : 1;
			if (count == 1) {
				switch (uniform.m_type) {
					case ShaderUniformType::BOOL:
					{
						m_shader->setUniformUInt(uniform.m_location, m_uniformStorageBuffer.read<uint32_t>(uniform.m_offset));
						break;
					}
					case ShaderUniformType::UINT:
					{
						m_shader->setUniformUInt(uniform.m_location, m_uniformStorageBuffer.read<uint32_t>(uniform.m_offset));
						break;
					}
					case ShaderUniformType::INT:
					{
						m_shader->setUniformInt(uniform.m_location, m_uniformStorageBuffer.read<int32_t>(uniform.m_offset));
						break;
					}
					case ShaderUniformType::FLOAT:
					{
						m_shader->setUniformFloat1(uniform.m_location, m_uniformStorageBuffer.read<float>(uniform.m_offset));
						break;
					}
					case ShaderUniformType::VEC2:
					{
						m_shader->setUniformFloat2(uniform.m_location, m_uniformStorageBuffer.read<glm::vec2>(uniform.m_offset));
						break;
					}
					case ShaderUniformType::VEC3:
					{
						m_shader->setUniformFloat3(uniform.m_location, m_uniformStorageBuffer.read<glm::vec3>(uniform.m_offset));
						break;
					}
					case ShaderUniformType::VEC4:
					{
						m_shader->setUniformFloat4(uniform.m_location, m_uniformStorageBuffer.read<glm::vec4>(uniform.m_offset));
						break;
					}
					case ShaderUniformType::MAT3:
					{
						m_shader->setUniformMatrix3(uniform.m_location, m_uniformStorageBuffer.read<glm::mat3>(uniform.m_offset));
						break;
					}
					case ShaderUniformType::MAT4:
					{
						m_shader->setUniformMatrix4(uniform.m_location, m_uniformStorageBuffer.read<glm::mat4>(uniform.m_offset));
						break;
					}
					default:
					{
						ASSERT(false, "Unknown ShaderUniformType");
						break;
					}
				}
			} else {
				switch (uniform.m_type) {
					case ShaderUniformType::BOOL:
					{
						m_shader->setUniformUInt(uniform.m_location, count, (uint32_t*)m_uniformStorageBuffer.at(uniform.m_offset));
						break;
					}
					case ShaderUniformType::UINT:
					{
						m_shader->setUniformUInt(uniform.m_location, count, (uint32_t*)m_uniformStorageBuffer.at(uniform.m_offset));
						break;
					}
					case ShaderUniformType::INT:
					{
						m_shader->setUniformInt(uniform.m_location, count, (int32_t*)m_uniformStorageBuffer.at(uniform.m_offset));
						break;
					}
					case ShaderUniformType::FLOAT:
					{
						m_shader->setUniformFloat1(uniform.m_location, count, (float*)m_uniformStorageBuffer.at(uniform.m_offset));
						break;
					}
					case ShaderUniformType::VEC2:
					{
						m_shader->setUniformFloat2(uniform.m_location, count, (glm::vec2*)m_uniformStorageBuffer.at(uniform.m_offset));
						break;
					}
					case ShaderUniformType::VEC3:
					{
						m_shader->setUniformFloat3(uniform.m_location, count, (glm::vec3*)m_uniformStorageBuffer.at(uniform.m_offset));
						break;
					}
					case ShaderUniformType::VEC4:
					{
						m_shader->setUniformFloat4(uniform.m_location, count, (glm::vec4*)m_uniformStorageBuffer.at(uniform.m_offset));
						break;
					}
					case ShaderUniformType::MAT3:
					{
						m_shader->setUniformMatrix3(uniform.m_location, count, (glm::mat3*)m_uniformStorageBuffer.at(uniform.m_offset));
						break;
					}
					case ShaderUniformType::MAT4:
					{
						m_shader->setUniformMatrix4(uniform.m_location, count, (glm::mat4*)m_uniformStorageBuffer.at(uniform.m_offset));
						break;
					}
					default:
						ASSERT(false, "Unknown ShaderUniformType");
						break;
				}
			}
		}
		PROFILE_GPU_END();
		PROFILE_END();
	}

	const ShaderUniform* Material::getShaderUniform(const std::string& name) {
		return m_shader->getUniform(name);
	}

	Ref<Texture> Material::getTexture(const std::string& name) const {
		auto it = m_textureBindings.find(name);
		if (it != m_textureBindings.end()) {
			return it->second.texture;
		}
		return nullptr;
	}
};