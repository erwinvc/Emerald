#pragma once
#include "../assets/metadata/assetMetadata.h"

namespace emerald {
	class Event {
		RTTI_BASE_CLASS_DECL(Event)
	public:
		virtual ~Event() = default;

		void setHandled() { m_handled = true; }
		bool isHandled() const { return m_handled; }

	private:
		bool m_handled = false;
	};

	class WindowResizeEvent : public Event {
		RTTI_DERIVED_CLASS_DECL(WindowResizeEvent, Event)

	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_width(width), m_height(height) {
		}

		uint32_t getWidth() const { return m_width; }
		uint32_t getHeight() const { return m_height; }
	private:
		unsigned int m_width, m_height;
	};

	class ErrorEvent : public Event {
		RTTI_DERIVED_CLASS_DECL(ErrorEvent, Event)

	public:
		ErrorEvent(const std::string& message, const std::string subMessage = "")
			: m_message(message), m_subMessage(subMessage) {
		}

		const std::string& getMessage() const { return m_message; }
		const std::string& getSubMessage() const { return m_subMessage; }

	private:
		std::string m_message;
		std::string m_subMessage;
	};

	class AssetStreamedEvent : public Event {
		RTTI_DERIVED_CLASS_DECL(AssetStreamedEvent, Event)

	public:
		AssetStreamedEvent(AssetMetadata* metadata, const Ref<Asset>& asset)
			: m_metadata(metadata), m_asset(asset) {
		}

		AssetMetadata* getMetadata() const { return m_metadata; }
		Ref<Asset> getAsset() const { return m_asset; }

	private:
		AssetMetadata* m_metadata;
		Ref<Asset> m_asset;
	};
}