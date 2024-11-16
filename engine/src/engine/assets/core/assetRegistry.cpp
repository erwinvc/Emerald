#include "eepch.h"
#include "assetRegistry.h"
#include "../texture/textureAsset.h"
#include "../../editor/src/project.h"
#include "utils/uuid/uuidGenerator.h"

namespace emerald {
	AssetRegistry::AssetRegistry() {
		m_assetTypeRegistry.registerAssetType<TextureAsset>(AssetType::TEXTURE, { ".png", ".bmp", ".jpg", ".jpeg" });
		//m_assetTypeRegistry.registerAssetType<MaterialAsset>(AssetType::MATERIAL, { ".mat" });
	}

	void AssetRegistry::parseCurrentProject() {
		clear();
		auto a = Project::GetAssetsPath();
		for (const auto& entry : std::filesystem::recursive_directory_iterator(a)) {
			if (entry.is_regular_file() && entry.path().has_extension() && entry.path().extension() != ".meta") {
				processAssetFile(entry.path());
			}
		}
	}

	void AssetRegistry::processAssetFile(const std::filesystem::path& assetPath) {
		if (!std::filesystem::exists(assetPath)) {
			return;
		}

		std::string extension = assetPath.extension().string();

		if(extension == ".meta") {
			return;
		}

		AssetType assetType = m_assetTypeRegistry.getAssetTypeFromExtension(extension);

		if (assetType == AssetType::UNKNOWN) {
			Log::warn("Unknown asset type for file: {}", assetPath.string());
			return;
		}

		std::filesystem::path metaFilePath = assetPath;
		metaFilePath.replace_extension(assetPath.extension().string() + ".meta");

		AssetMetadata metadata;

		if (std::filesystem::exists(metaFilePath)) {
			Log::info("Parsing existing .meta file: {}", metaFilePath.string());
			metadata = AssetMetadata::deserializeFromFile(metaFilePath);
		} else {
			Log::info("Creating new .meta file for asset: {}", assetPath.string());

			metadata = AssetMetadata::create(UUIDGenerator::create(), );
			metadata.serializeToFile(metaFilePath);
		}

		m_assets.push_back(metadata);
		AssetMetadata* m_metadataRef = &m_assets.back();
		m_pathAssetMap.insert({ assetPath, m_metadataRef });
		m_typeAssetMap.insert({ metadata.getType(), m_metadataRef });
		m_uuidAssetMap.insert({ metadata.getUUID(), m_metadataRef });
	}
}