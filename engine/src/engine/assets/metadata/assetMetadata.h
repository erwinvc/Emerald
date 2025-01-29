#pragma once
#include "core/common/engineConstants.h"
#include "utils/uuid/uuid.h"
#include "utils/text/jsonUtils.h"
#include "engine/assets/core/asset.h"
#include "engine/assets/loaders/assetLoader.h"

namespace emerald {
	enum class AssetLoadedState {
		NOTLOADED,
		LOADING,
		LOADED
	};

	class AssetMetadata {
	public:
		AssetMetadata(AssetType type, const std::filesystem::path& path);
		virtual ~AssetMetadata() = default;

		//template <typename T>
		//Ref<T> getAsset() {
		//	if (m_asset.expired()) {
		//		m_loadedState = AssetLoadedState::NOTLOADED;
		//	}
		//	if (m_loadedState == AssetLoadedState::NOTLOADED) {
		//		m_loadedState = AssetLoadedState::LOADING;
		//		loadAsset();
		//	}
		//	return m_asset.lock();
		//}

		virtual Ref<AssetLoader> createAssetLoader() = 0;

		AssetLoadedState getLoadedState() const { return m_loadedState; }

		nlohmann::json toJson() const {
			nlohmann::json j;
			j["version"] = EngineConstants::VERSION_ASSETMETADATA;
			j["uuid"] = m_uuid;
			return j;
		}

		void fromJson(const nlohmann::json& j) {
			int version = jsonUtils::deserializeRequiredValue<int>(j, "version");
			if (version != EngineConstants::VERSION_ASSETMETADATA) {
				throw jsonUtils::VersionMismatchError(EngineConstants::VERSION_ASSETMETADATA, version);
			}

			m_uuid = jsonUtils::deserializeRequiredValue<std::string>(j, "uuid");
		}

		AssetType getType() const {
			return m_type;
		}

		UUID getUUID() const {
			return m_uuid;
		}

		std::filesystem::path getPath() const {
			return m_path;
		}

	protected:
		UUID m_uuid;
		AssetType m_type;
		WeakRef<Asset> m_asset;
		std::filesystem::path m_path;
		AssetLoadedState m_loadedState = AssetLoadedState::NOTLOADED;
	};
}