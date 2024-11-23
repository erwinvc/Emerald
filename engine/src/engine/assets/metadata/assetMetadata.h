#pragma once
#include "core/common/engineConstants.h"
#include "core/common/engineError.h"
#include "utils/uuid/uuid.h"
#include "core/common/ref.h"
#include "core/common/jsonSerializable.h"
#include "engine/assets/core/asset.h"

namespace emerald {
	class AssetMetadata {
	public:
		virtual ~AssetMetadata() = default;

		template <typename T>
		Ref<T> getAsset() {
			return m_asset.lock();
		}

		nlohmann::json toJson() const {
			nlohmann::json j;
			j["version"] = editorConstants::VERSION;
			j["uuid"] = m_uuid;
			return j;
		}

		static void fromJson(const nlohmann::json& j, AssetMetadata& metaData) {
			int version = jsonUtils::deserializeRequiredValue<int>(j, "version");
			if (version != editorConstants::VERSION) {
				throw jsonUtils::VersionMismatchError(editorConstants::VERSION, version);
			}

			metaData.m_uuid = jsonUtils::deserializeRequiredValue<std::string>(j, "uuid");
		}

		static AssetMetadata create(const UUID& uuid, AssetType type) {
			AssetMetadata metaData;
			metaData.m_uuid = uuid;
			metaData.m_type = type;
			return metaData;
		}

		AssetType getType() const {
			return m_type;
		}

		UUID getUUID() const {
			return m_uuid;
		}

	private:
		UUID m_uuid;
		AssetType m_type;
		WeakRef<Asset> m_asset;
	};
}