#pragma once
#include "scene.h"

namespace emerald {
	class SceneSerialization {
	public:
		static void serializeScene(const std::filesystem::path& path, const Ref<Scene>& scene);
		static Ref<Scene> deserializeScene(const std::filesystem::path& path);
	};
}