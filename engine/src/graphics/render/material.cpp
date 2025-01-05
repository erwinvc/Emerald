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

		PROFILE_LOGIC_BEGIN("Update material");

		int textureSlot = 0;

		for (const auto& [name, binding] : m_textureBindings) {
			if (binding.texture) {
				binding.texture->bind(binding.unit);
			}
		}

		Renderer::submit([buffer = m_uniformStorageBuffer, shader = Ref<Shader>(m_shader)] {
			const auto& uniformBuffers = shader->getUniformBuffers();
			for (auto& [name, uniform] : uniformBuffers) {
				uint32_t count = uniform.m_isArray ? uniform.m_count : 1;

				if (count == 1) {
					switch (uniform.m_type) {
						case ShaderUniformType::BOOL:
						{
							shader->setUniformUInt(uniform.m_location, buffer.read<uint32_t>(uniform.m_offset));
							break;
						}
						case ShaderUniformType::UINT:
						{
							shader->setUniformUInt(uniform.m_location, buffer.read<uint32_t>(uniform.m_offset));
							break;
						}
						case ShaderUniformType::INT:
						{
							shader->setUniformInt(uniform.m_location, buffer.read<int32_t>(uniform.m_offset));
							break;
						}
						case ShaderUniformType::FLOAT:
						{
							shader->setUniformFloat1(uniform.m_location, buffer.read<float>(uniform.m_offset));
							break;
						}
						case ShaderUniformType::VEC2:
						{
							shader->setUniformFloat2(uniform.m_location, buffer.read<glm::vec2>(uniform.m_offset));
							break;
						}
						case ShaderUniformType::VEC3:
						{
							shader->setUniformFloat3(uniform.m_location, buffer.read<glm::vec3>(uniform.m_offset));
							break;
						}
						case ShaderUniformType::VEC4:
						{
							shader->setUniformFloat4(uniform.m_location, buffer.read<glm::vec4>(uniform.m_offset));
							break;
						}
						case ShaderUniformType::MAT3:
						{
							shader->setUniformMatrix3(uniform.m_location, buffer.read<glm::mat3>(uniform.m_offset));
							break;
						}
						case ShaderUniformType::MAT4:
						{
							shader->setUniformMatrix4(uniform.m_location, buffer.read<glm::mat4>(uniform.m_offset));
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
							shader->setUniformUInt(uniform.m_location, count, (uint32_t*)buffer.at(uniform.m_offset));
							break;
						}
						case ShaderUniformType::UINT:
						{
							shader->setUniformUInt(uniform.m_location, count, (uint32_t*)buffer.at(uniform.m_offset));
							break;
						}
						case ShaderUniformType::INT:
						{
							shader->setUniformInt(uniform.m_location, count, (int32_t*)buffer.at(uniform.m_offset));
							break;
						}
						case ShaderUniformType::FLOAT:
						{
							shader->setUniformFloat1(uniform.m_location, count, (float*)buffer.at(uniform.m_offset));
							break;
						}
						case ShaderUniformType::VEC2:
						{
							shader->setUniformFloat2(uniform.m_location, count, (glm::vec2*)buffer.at(uniform.m_offset));
							break;
						}
						case ShaderUniformType::VEC3:
						{
							shader->setUniformFloat3(uniform.m_location, count, (glm::vec3*)buffer.at(uniform.m_offset));
							break;
						}
						case ShaderUniformType::VEC4:
						{
							shader->setUniformFloat4(uniform.m_location, count, (glm::vec4*)buffer.at(uniform.m_offset));
							break;
						}
						case ShaderUniformType::MAT3:
						{
							shader->setUniformMatrix3(uniform.m_location, count, (glm::mat3*)buffer.at(uniform.m_offset));
							break;
						}
						case ShaderUniformType::MAT4:
						{
							shader->setUniformMatrix4(uniform.m_location, count, (glm::mat4*)buffer.at(uniform.m_offset));
							break;
						}
						default:
							ASSERT(false, "Unknown ShaderUniformType");
							break;
					}
				}
			}
		});
		PROFILE_LOGIC_END();
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