#pragma once
#include "events.h"
#include <chrono>
#include <core/common/rtti.h>
#include <filesystem>
#include <system_error>

namespace emerald {
	enum class FileChangedEventType {
		DELETED,
		CREATED,
		MODIFIED,
		_COUNT
	};

	class FileChangedEvent : public Event {
		RTTI_DERIVED_CLASS_DECL(FileChangedEvent, Event)

	public:
		FileChangedEvent() = default;
		FileChangedEvent(FileChangedEventType type, const std::filesystem::path& path, const std::chrono::system_clock::time_point& timestamp, bool isEngineAsset)
			: type(type), path(path), timestamp(timestamp), m_isEngineAsset(isEngineAsset) {
		}

		FileChangedEventType getType() const { return type; }
		const std::filesystem::path& getPath() const { return path; }
		const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp; }
		bool isEngineAsset() const { return m_isEngineAsset; }

	private:
		FileChangedEventType type;
		std::filesystem::path path;
		std::chrono::system_clock::time_point timestamp;
		bool m_isEngineAsset = false;
	};
}