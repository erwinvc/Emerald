#pragma once
#include <filesystem>
#include "utils/system/timestep.h"
#include "../ecs/core/entityComponentSystem.h"
#include "graphics/core/camera.h"

namespace emerald {
	class SceneGraphComponent;

	class Scene : public RefCounted {
	public:
		Scene(const std::filesystem::path& path, UUID sceneRootID);
		~Scene() = default;
		
		void load();
		void save();
		void update(Timestep ts);
		void getActiveCamera();
		void updateAllTransforms();
		const std::string& getName() const { return m_name; }
		SceneGraphComponent* getRootNode() { return m_sceneRoot; }
		bool isDirty() const { return m_isDirty; }
		void setDirty() { m_isDirty = true; }

	protected:
		std::string m_name;
		std::filesystem::path m_path;
		WeakRef<Camera> m_activeCamera;
		SceneGraphComponent* m_sceneRoot;
		bool m_isDirty = false;

		void updateTransformsRecursively(SceneGraphComponent* node, const glm::mat4& parentTransform);
	};
}