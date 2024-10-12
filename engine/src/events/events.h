#pragma once
#include "rtti.h"

namespace emerald {
	class Event {
		RTTI_BASE_CLASS_DECL(Event)
	public:
		void setHandled() { m_handled = true; }
		bool isHandled() const { return m_handled; }

	private:
		bool m_handled = false;
	};

	class WindowResizeEvent : public Event {
		RTTI_DERIVED_CLASS_DECL(WindowResizeEvent, Event)

		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_width(width), m_height(height) {
		}

		inline uint32_t getWidth() const { return m_width; }
		inline uint32_t getHeight() const { return m_height; }
	private:
		unsigned int m_width, m_height;
	};
}