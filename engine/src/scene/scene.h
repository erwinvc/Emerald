#pragma once
#include "../util/timestep.h"
#include "graphics/camera.h"
#include <filesystem>
#include "entities/entityComponentSystem.h"

namespace emerald {
	class Scene : public RefCounted {
	public:
		Scene(const std::string name, const std::filesystem::path& scenePath);
		~Scene() = default;
		
		void initialize();
		void load();
		void save();
		void update(Timestep ts);
		void getActiveCamera();
		const std::string& getName() const { return m_name; }
		EntityComponentSystem& getECS() { return m_ecs; }

	protected:
		std::string m_name;
		std::filesystem::path m_path;
		EntityComponentSystem m_ecs;
		WeakRef<Camera> m_activeCamera;
	};
}