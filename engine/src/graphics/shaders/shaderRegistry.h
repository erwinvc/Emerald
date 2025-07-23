#pragma once
#include "shader.h"
#include "utils\text\stringUtils.h"
#include "engine\events\fileChangedEvent.h"

namespace emerald {
	class ShaderRegistry {
	public:
		static void registerShader(const WeakRef<Shader>& shader) {
			if (shader.expired()) {
				throw std::runtime_error("Cannot register expired shader");
			}
			s_shaders.push_back(shader);
			std::string name = StringUtils::toLower(shader.lock()->getName());
			s_shadersMap[name] = shader;
		}

		static void unregisterShader(Shader* shader) {
			auto it = s_shadersMap.find(shader->getName());
			if (it != s_shadersMap.end()) {
				s_shadersMap.erase(it);
			}
			for (auto it = s_shaders.begin(); it != s_shaders.end(); it++) {
				if (it->expired()) continue;
				if (it->raw() == shader) {
					s_shaders.erase(it);
					break;
				}
			}
		}

		static Ref<Shader> getShader(const std::string& name) {
			std::string toFind = StringUtils::toLower(name);
			auto it = s_shadersMap.find(toFind);
			if (it != s_shadersMap.end()) {
				if (it->second.expired()) {
					throw EmeraldError("Shader is expired");
				}
				return it->second.lock();
			}

			throw EmeraldError("Shader '{}' not found", name);
		}

		static std::vector<Ref<Shader>> getShaders() {
			std::vector<Ref<Shader>> shaders;
			for (auto& shader : s_shaders) {
				if (shader.expired()) continue;
				shaders.push_back(shader.lock());
			}
			return shaders;
		}

		static inline std::vector<WeakRef<Shader>> s_shaders;
		static inline std::unordered_map<std::string, WeakRef<Shader>> s_shadersMap;
	};

	class InternalShaders {
	public:
		void initialize();
	private:
		std::vector<Ref<Shader>> m_shaders;
		std::unordered_map<std::filesystem::path, Ref<Shader>> m_shadersMap;

		void onFileChangedEvent(FileChangedEvent& e);
	};
}