#pragma once
#include "core/common/rtti.h"
#include "engine/events/events.h"

namespace emerald {
	class EditorProjectOpenedEvent : public Event {
		RTTI_DERIVED_CLASS_DECL(EditorProjectOpenedEvent, Event);

	public:
		EditorProjectOpenedEvent(bool isValid) : m_isValid(isValid) {}
		bool isValid() const { return m_isValid; }

	private:
		bool m_isValid;
	};
}