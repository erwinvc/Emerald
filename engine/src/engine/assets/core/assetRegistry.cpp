#include "eepch.h"
#include "assetRegistry.h"
#include "assetMetadata.h"
#include "utils/uuid/uuidGenerator.h"
#include "../texture/textureAsset.h"

namespace emerald {
	AssetRegistry::AssetRegistry() {
		m_assetTypeRegistry.registerAssetType<TextureAsset>("texture", { ".png", ".bmp", ".jpg", ".jpeg" });
	}

	void AssetRegistry::setDirectory(std::filesystem::path dir) {
		m_trackedAssets.clear();
		for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
			if (entry.is_regular_file() && entry.path().has_extension() && entry.path().extension() != ".meta") {
				processAssetFile(entry.path());
			}
		}
	}

	void AssetRegistry::processAssetFile(const std::filesystem::path& path) {
		//std::string ext = path.extension().string();
		//std::filesystem::path metaPath = path.string() + ".meta";
		//AssetMetadata metadata;
		//
		//if (std::filesystem::exists(metaPath)) {
		//	std::ifstream metaFile(metaPath);
		//	nlohmann::json j;
		//	metaFile >> j;
		//	metadata = AssetMetadata::deserialize(j);
		//} else {
		//	metadata.m_uuid = UUIDGenerator::create();
		//	std::ofstream metaFile(metaPath);
		//	metaFile << metadata.serialize().dump(4);
		//}
		//
		//m_trackedAssets[metadata.m_uuid] = metadata;
	}

	void AssetRegistry::garbageCollect() {
		for (auto it = m_trackedAssets.begin(); it != m_trackedAssets.end(); ) {
			if (it->second->m_refCount.load() == 1) {
				it = m_trackedAssets.erase(it);
			} else {
				++it;
			}
		}
	}
}