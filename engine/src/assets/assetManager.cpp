#include "eepch.h"
#include "assetManager.h"

namespace emerald {
	void AssetManager::garbageCollect() {
		for (auto it = m_loadedAssets.begin(); it != m_loadedAssets.end(); ) {
			if (it->second->m_refCount.load() == 1) {
				it = m_loadedAssets.erase(it);
			} else {
				++it;
			}
		}
	}
}