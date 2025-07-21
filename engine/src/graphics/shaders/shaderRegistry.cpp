#include "eepch.h"
#include "shaderRegistry.h"
#include "engine\events\eventSystem.h"

namespace emerald {
	void InternalShaders::initialize() {
		EventSystem::subscribe<FileChangedEvent>(&InternalShaders::onFileChangedEvent, this);
		for (const auto& entry : std::filesystem::directory_iterator("res/shaders")) {
			auto absolutePath = std::filesystem::absolute(entry.path());
			Ref<Shader> shader = Ref<Shader>::create(absolutePath);
			if (shader->isValid()) {
				m_shaders.push_back(shader);
				m_shadersMap[absolutePath] = shader;
				ShaderRegistry::registerShader(shader);
			} else {
				Log::error("Failed to load engine shader: {}", absolutePath.filename().string());
			}
		}
	}

	void InternalShaders::onFileChangedEvent(FileChangedEvent& e) {
		if (!e.isEngineAsset()) return;
		switch (e.getType()) {
			case FileChangedEventType::CREATED:
			case FileChangedEventType::MODIFIED:
			{
				auto absolutePath = std::filesystem::absolute(e.getPath());
				auto it = m_shadersMap.find(absolutePath);
				if (it != m_shadersMap.end()) {
					if (it->second->reload()) {
						Log::info("[InternalShaders] Reloaded internal shader: {}", it->second->getName());
					} else {
						Log::error("[InternalShaders] Failed to reload internal shader: {}", it->second->getName());
					}
				}
			}
		}
	}
}