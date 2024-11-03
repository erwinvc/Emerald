#pragma once

namespace emerald {
	class AssetMetadata {
	public:
		UUID m_uuid;

		nlohmann::json serialize() const {
			return {
				{"uuid", m_uuid}
			};
		}

		static AssetMetadata deserialize(const nlohmann::json& j) {
			AssetMetadata metadata;
			metadata.m_uuid = UUID(j["uuid"]);
			return metadata;
		}
	};
}