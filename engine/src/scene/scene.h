#pragma once
#include "../util/timestep.h"

namespace emerald {
	class Scene : public RefCounted {
	public:
		Scene(const std::filesystem::path& scenePath)
			: path(scenePath) {
		}

		virtual ~Scene() = default;

		virtual void load() = 0; 
		virtual void save() = 0;
		virtual void update(Timestep ts) = 0;

	protected:
		std::filesystem::path path;
	};

}