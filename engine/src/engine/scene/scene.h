#pragma once
#include <filesystem>
#include "utils/system/timestep.h"
#include "../ecs/core/entityComponentSystem.h"
#include "graphics/core/camera.h"

namespace emerald {
	class SceneGraphComponent;

	class Scene : public RefCounted {
	public:
		Scene(const std::string name, const std::filesystem::path& scenePath);
		~Scene() = default;
		
		void load();
		void save();
		void update(Timestep ts);
		void getActiveCamera();
		void updateAllTransforms();
		const std::string& getName() const { return m_name; }
		EntityComponentSystem& getECS() { return m_ecs; }
		SceneGraphComponent* getRootNode() { return m_sceneRoot; }

	protected:
		std::string m_name;
		std::filesystem::path m_path;
		EntityComponentSystem m_ecs;
		WeakRef<Camera> m_activeCamera;
		SceneGraphComponent* m_sceneRoot;

		void updateTransformsRecursively(SceneGraphComponent* node, const glm::mat4& parentTransform);
	};
}