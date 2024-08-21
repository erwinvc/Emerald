#include "eepch.h"
#include "material.h"
#include "shaders/shader.h"

namespace emerald {
	Material::Material(std::string name, Ref<Shader> shader)
		: m_name(name), m_shader(shader) {

		initializeBuffer();
	}

	void Material::initializeBuffer() {
		ASSERT(m_shader->isLoaded(), "Trying to create a material for a shader that is not yet loaded");
		const auto& uniformBuffers = m_shader->getUniformBuffers();
		uint32_t size = 0;

		Log::info("[Material] Created '{}' material with {}", m_name, (uniformBuffers.size() == 0 ? "no uniforms" : "uniforms: "));

		for (auto& [name, buffer] : uniformBuffers) {
			Log::info("[Material] {}", name);
			size += buffer.m_size * buffer.m_count;
		}

		m_uniformStorageBuffer.reserve(size);
		m_uniformStorageBuffer.zeroInitialize();
	}

	void Material::updateForRendering() {
		m_shader->bind();

		const auto& uniformBuffers = m_shader->getUniformBuffers();

		for (auto& [name, buffer] : uniformBuffers) {
			switch (buffer.m_type) {
				case ShaderUniformType::BOOL:
				case ShaderUniformType::UINT: m_shader->setUniformUInt(buffer.m_location, buffer.m_count, (uint32_t*)&m_uniformStorageBuffer.at(buffer.m_offset)); break;
				case ShaderUniformType::INT: m_shader->setUniformInt(buffer.m_location, buffer.m_count, (int*)&m_uniformStorageBuffer.at(buffer.m_offset)); break;
				case ShaderUniformType::FLOAT: m_shader->setUniformFloat1(buffer.m_location, buffer.m_count, (float*)&m_uniformStorageBuffer.at(buffer.m_offset)); break;
				case ShaderUniformType::VEC2: m_shader->setUniformFloat2(buffer.m_location, buffer.m_count, (float*)&m_uniformStorageBuffer.at(buffer.m_offset)); break;
				case ShaderUniformType::VEC3: m_shader->setUniformFloat3(buffer.m_location, buffer.m_count, (float*)&m_uniformStorageBuffer.at(buffer.m_offset)); break;
				case ShaderUniformType::VEC4: m_shader->setUniformFloat4(buffer.m_location, buffer.m_count, (float*)&m_uniformStorageBuffer.at(buffer.m_offset));	break;
				case ShaderUniformType::MAT3: m_shader->setUniformMatrix3(buffer.m_location, buffer.m_count, (float*)&m_uniformStorageBuffer.at(buffer.m_offset)); break;
				case ShaderUniformType::MAT4: m_shader->setUniformMatrix4(buffer.m_location, buffer.m_count, (float*)&m_uniformStorageBuffer.at(buffer.m_offset)); break;
				default:
				{
					ASSERT(false, "Unknown ShaderUniformType");
					break;
				}
			}
		}
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