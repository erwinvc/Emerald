#pragma once
#include "engine\ecs\core\entity.h"
#include "utils\datastructures\vector.h"

namespace emerald {
	class Selection {
	public:
		static const Vector<Entity>& get();
		static void set(const Vector<Entity>&);  
		static void toggle(Entity);          
		static bool contains(Entity);
		static void clear();

	private:
		static void notify();
		static inline Vector<Entity> m_selected;
		static inline std::mutex     m_mutex;
	};
}