#pragma once
#include "asset.h"
#include "utils/uuid/uuid.h"

namespace emerald {
	class AssetManager {
	public:
		void garbageCollect();

	private:
		std::unordered_map<UUID, Ref<Asset>> m_loadedAssets;
	};
}