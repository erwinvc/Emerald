#pragma once
#include "core/common/rtti.h"
#include "engine/events/events.h"

#include "engine/ecs/core/entity.h"
#include "utils/datastructures/vector.h"

namespace emerald {
	// Fired every time the global selection changes
	class SelectionChangedEvent : public Event {
		RTTI_DERIVED_CLASS_DECL(SelectionChangedEvent, Event)

	public:
		explicit SelectionChangedEvent(Vector<Entity> selection)
			: m_selection(std::move(selection)) {
		}

		const Vector<Entity>& getSelection() const { return m_selection; }

	private:
		Vector<Entity> m_selection;
	};
}