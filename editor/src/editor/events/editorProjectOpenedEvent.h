#pragma once
#include "core/common/rtti.h"
#include "engine/events/events.h"

namespace emerald {
	class EditorProjectOpenedEvent : public Event {
		RTTI_DERIVED_CLASS_DECL(EditorProjectOpenedEvent, Event)

	public:
		EditorProjectOpenedEvent(const std::filesystem::path& path, bool isValid) : m_path(path), m_isValid(isValid) {}

		const std::filesystem::path& getPath() const { return m_path; }
		bool isValid() const { return m_isValid; }

	private:
		bool m_isValid;
		std::filesystem::path m_path;
	};
}