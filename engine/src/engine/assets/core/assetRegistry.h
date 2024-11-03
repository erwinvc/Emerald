#pragma once
#include "asset.h"
#include "utils/uuid/uuid.h"
#include "assetTypeRegistry.h"

namespace emerald {
	class AssetRegistry {
	public:
		AssetRegistry();
		void garbageCollect();
		void setDirectory(const std::filesystem::path dir);
		void processAssetFile(const std::filesystem::path& path);
		void clear() { m_trackedAssets.clear(); }

	private:
		AssetTypeRegistry m_assetTypeRegistry;
		std::unordered_map<UUID, Ref<Asset>> m_trackedAssets;
	};
}