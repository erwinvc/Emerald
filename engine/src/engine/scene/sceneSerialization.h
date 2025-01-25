#pragma once
#include "scene.h"

namespace emerald {
	class SceneSerialization {
	public:
		static constexpr std::string_view VERSION_KEY = "1version";
		static constexpr std::string_view SCENE_KEY = "2scene";

		static void serializeScene(const std::filesystem::path& path, const Ref<Scene>& scene);
		static Ref<Scene> deserializeScene(const std::filesystem::path& path);
	};
}