#include "eepch.h"
#include "selection.h"

namespace emerald{
	Vector<Entity> Selection::getSelectedEntities() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_selectedEntities;
	}

	void Selection::selectEntity(Entity entity) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_selectedEntities.pushBack(entity);
	}

	void Selection::deselectEntity(Entity entity) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_selectedEntities.remove(entity);
	}

	void Selection::clearSelection() {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_selectedEntities.clear();
	}
}