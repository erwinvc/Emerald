#pragma once

namespace emerald {
	class Event {
		RTTI_BASE_CLASS_DECL(Event);
	public:
		virtual ~Event() = default;

		void setHandled() { m_handled = true; }
		bool isHandled() const { return m_handled; }

	private:
		bool m_handled = false;
	};

	class WindowResizeEvent : public Event {
		RTTI_DERIVED_CLASS_DECL(WindowResizeEvent, Event);

		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_width(width), m_height(height) {
		}

		inline uint32_t getWidth() const { return m_width; }
		inline uint32_t getHeight() const { return m_height; }
	private:
		unsigned int m_width, m_height;
	};

	class ErrorEvent : public Event {
		RTTI_DERIVED_CLASS_DECL(ErrorEvent, Event);
		~ErrorEvent() {
			Log::info("Del");
		}
		ErrorEvent(const std::string& message, const std::string subMessage = "")
			: m_message(message), m_subMessage(subMessage) {
		}

		inline const std::string& getMessage() const { return m_message; }
		inline const std::string& getSubMessage() const { return m_subMessage; }

	private:
		std::string m_message;
		std::string m_subMessage;
	};
}