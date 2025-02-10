#pragma once
#include "engine\ecs\core\entity.h"
#include "utils\datastructures\vector.h"

namespace emerald {
	class Selection {
	public:
		static Vector<Entity> getSelectedEntities();
		static void selectEntity(Entity entity);
		static void deselectEntity(Entity entity);
		static void clearSelection();

	private:
		static inline std::mutex m_mutex;
		static inline Vector<Entity> m_selectedEntities;
	};
}