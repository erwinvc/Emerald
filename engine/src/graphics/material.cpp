#include "eepch.h"
#include "material.h"
#include "shaders/shader.h"
#include "imguiProfiler/Profiler.h"

namespace emerald {
	Material::Material(std::string name, const Ref<Shader>& shader)
		: m_name(name), m_shader(shader) {

		initializeBuffer();
	}

	void Material::initializeBuffer() {
		//ASSERT(m_shader->isLoaded(), "Trying to create a material for a shader that is not yet loaded");
		//const auto& uniformBuffers = m_shader->getUniformBuffers();
		//uint32_t totalSize = 0;
		//
		//for (auto& [name, uniform] : uniformBuffers) {
		//	totalSize += uniform.m_size * uniform.m_count;
		//}
		//
		//m_uniformStorageBuffer.reserve(totalSize);
		//m_uniformStorageBuffer.zeroInitialize();

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
		const auto& uniformBuffers = m_shader->getUniformBuffers();

		for (auto& [name, uniform] : uniformBuffers) {
			uint32_t count = uniform.m_isArray ? uniform.m_count : 1;

			if (count == 1) {
				switch (uniform.m_type) {
					case ShaderUniformType::BOOL:
					{
						auto value = m_uniformStorageBuffer.read<uint32_t>(uniform.m_offset);
						m_shader->setUniformUInt(uniform.m_location, uniform.m_count, value);
						break;
					}
					case ShaderUniformType::UINT:
					{
						auto value = m_uniformStorageBuffer.read<uint32_t>(uniform.m_offset);
						m_shader->setUniformUInt(uniform.m_location, uniform.m_count, value);
						break;
					}
					case ShaderUniformType::INT:
					{
						auto value = m_uniformStorageBuffer.read<int32_t>(uniform.m_offset);
						m_shader->setUniformInt(uniform.m_location, uniform.m_count, value);
						break;
					}
					case ShaderUniformType::FLOAT:
					{
						auto value = m_uniformStorageBuffer.read<float>(uniform.m_offset);
						m_shader->setUniformFloat1(uniform.m_location, uniform.m_count, value);
						break;
					}
					case ShaderUniformType::VEC2:
					{
						auto value = m_uniformStorageBuffer.read<glm::vec2>(uniform.m_offset);
						m_shader->setUniformFloat2(uniform.m_location, uniform.m_count, value);
						break;
					}
					case ShaderUniformType::VEC3:
					{
						auto value = m_uniformStorageBuffer.read<glm::vec3>(uniform.m_offset);
						m_shader->setUniformFloat3(uniform.m_location, uniform.m_count, value);
						break;
					}
					case ShaderUniformType::VEC4:
					{
						auto value = m_uniformStorageBuffer.read<glm::vec4>(uniform.m_offset);
						m_shader->setUniformFloat4(uniform.m_location, uniform.m_count, value);
						break;
					}
					case ShaderUniformType::MAT3:
					{
						auto value = m_uniformStorageBuffer.read<glm::mat3>(uniform.m_offset);
						m_shader->setUniformMatrix3(uniform.m_location, uniform.m_count, value);
						break;
					}
					case ShaderUniformType::MAT4:
					{
						auto value = m_uniformStorageBuffer.read<glm::mat4>(uniform.m_offset);
						m_shader->setUniformMatrix4(uniform.m_location, uniform.m_count, value);
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
						std::vector<uint32_t> values(count);
						m_uniformStorageBuffer.readArray(values.data(), uniform.m_offset, count);
						m_shader->setUniformUInt(uniform.m_location, count, values);
						break;
					}
					case ShaderUniformType::UINT:
					{
						std::vector<uint32_t> values(count);
						m_uniformStorageBuffer.readArray(values.data(), uniform.m_offset, count);
						m_shader->setUniformUInt(uniform.m_location, count, values);
						break;
					}
					case ShaderUniformType::INT:
					{
						std::vector<int32_t> values(count);
						m_uniformStorageBuffer.readArray(values.data(), uniform.m_offset, count);
						m_shader->setUniformInt(uniform.m_location, count, values);
						break;
					}
					case ShaderUniformType::FLOAT:
					{
						std::vector<float> values(count);
						m_uniformStorageBuffer.readArray(values.data(), uniform.m_offset, count);
						m_shader->setUniformFloat1(uniform.m_location, count, values);
						break;
					}
					case ShaderUniformType::VEC2:
					{
						std::vector<glm::vec2> values(count);
						m_uniformStorageBuffer.readArray(values.data(), uniform.m_offset, count);
						m_shader->setUniformFloat2(uniform.m_location, count, values);
						break;
					}
					case ShaderUniformType::VEC3:
					{
						std::vector<glm::vec3> values(count);
						m_uniformStorageBuffer.readArray(values.data(), uniform.m_offset, count);
						m_shader->setUniformFloat3(uniform.m_location, count, values);
						break;
					}
					case ShaderUniformType::VEC4:
					{
						std::vector<glm::vec4> values(count);
						m_uniformStorageBuffer.readArray(values.data(), uniform.m_offset, count);
						m_shader->setUniformFloat4(uniform.m_location, count, values);
						break;
					}
					case ShaderUniformType::MAT3:
					{
						std::vector<glm::mat3> values(count);
						m_uniformStorageBuffer.readArray(values.data(), uniform.m_offset, count);
						m_shader->setUniformMatrix3(uniform.m_location, count, values);
						break;
					}
					case ShaderUniformType::MAT4:
					{
						std::vector<glm::mat4> values(count);
						m_uniformStorageBuffer.readArray(values.data(), uniform.m_offset, count);
						m_shader->setUniformMatrix4(uniform.m_location, count, values);
						break;
					}
					default:
						ASSERT(false, "Unknown ShaderUniformType");
						break;
				}
			}
		}
		PROFILE_LOGIC_END();
	}

	const ShaderUniform* Material::getShaderUniform(const std::string& name) {
		const auto& uniformBuffers = m_shader->getUniformBuffers();
		auto it = uniformBuffers.find(name);
		if (it != uniformBuffers.end()) {
			return &it->second;
		}
		return nullptr;
	}
};