#pragma once
#include "asset.h"
#include "assetTypeRegistry.h"
#include "engine/assets/metadata/assetMetadata.h"
#include "utils/uuid/uuid.h"

namespace emerald {
	class AssetRegistry {
	public:
		AssetRegistry();

		void parseCurrentProject();
		void registerAsset(UniqueRef<AssetMetadata> metadata, const std::filesystem::path& path);
		void processAssetFile(const std::filesystem::path& assetPath);

		template<typename T>
		Ref<T> getAsset(const UUID& uuid) {
			auto it = m_uuidAssetMap.find(uuid);
			if (it != m_uuidAssetMap.end()) {
				return it->second.getAsset();
			}
			return nullptr;
		}

		AssetMetadata* getAssetMetadata(const UUID& uuid) {
			auto it = m_uuidAssetMap.find(uuid);
			if (it != m_uuidAssetMap.end()) {
				return it->second;
			}
			return nullptr;
		}

		AssetMetadata* getAssetMetadata(const std::filesystem::path& path) {
			auto it = m_pathAssetMap.find(path);
			if (it != m_pathAssetMap.end()) {
				return it->second;
			}
			return nullptr;
		}

		void clear() {
			m_assets.clear();
			m_uuidAssetMap.clear();
			m_pathAssetMap.clear();
		}

	private:
		AssetTypeRegistry m_assetTypeRegistry;
		std::vector<UniqueRef<AssetMetadata>> m_assets;
		std::unordered_map<UUID, AssetMetadata*> m_uuidAssetMap;
		std::unordered_map<std::filesystem::path, AssetMetadata*> m_pathAssetMap;
	};
}