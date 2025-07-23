#include "eepch.h"
#include "selection.h"
#include "engine\events\eventSystem.h"
#include "editor\events\selectionChangedEvent.h"

namespace emerald {
	const Vector<Entity>& Selection::get() { return m_selected; }
	bool Selection::contains(Entity e) {
		std::lock_guard lock(m_mutex);
		return std::find(m_selected.begin(), m_selected.end(), e) != m_selected.end();
	}

	void Selection::set(const Vector<Entity>& set) {
		std::lock_guard lock(m_mutex);
		m_selected = set;
		std::sort(m_selected.begin(), m_selected.end());
		notify();
	}

	void Selection::toggle(Entity e) {
		std::lock_guard lock(m_mutex);
		auto it = std::find(m_selected.begin(), m_selected.end(), e);
		if (it == m_selected.end()) m_selected.pushBack(e);
		else m_selected.erase(it);
		notify();
	}

	void Selection::clear() {
		std::lock_guard lock(m_mutex);
		m_selected.clear();
		notify();
	}

	void Selection::notify() {
		EventSystem::dispatch<SelectionChangedEvent>(m_selected);
	}
}