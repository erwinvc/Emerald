#pragma once
#include "util/uuid.h"
#include "asset.h"

namespace emerald {
	class AssetManager {
	public:
		void garbageCollect();

	private:
		std::unordered_map<UUID, Ref<Asset>> m_loadedAssets;
	};
}