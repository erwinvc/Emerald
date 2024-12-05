#include "eepch.h"
#include "assetRegistry.h"
#include "../texture/textureAsset.h"
#include "../../editor/src/project.h"
#include "utils/uuid/uuidGenerator.h"

namespace emerald {
	AssetRegistry::AssetRegistry() {
		m_assetTypeRegistry.registerType<TextureAsset>(AssetType::TEXTURE, { ".png", ".bmp", ".jpg", ".jpeg" });
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

	void AssetRegistry::registerAsset(UniqueRef<AssetMetadata> metadata, const std::filesystem::path& path) {
		m_assets.push_back(std::move(metadata));

		m_uuidAssetMap[metadata->getUUID()] = metadata.raw();
		m_typeAssetMap[metadata->getType()] = metadata.raw();
		m_pathAssetMap[path] = metadata.raw();
	}

	void AssetRegistry::processAssetFile(const std::filesystem::path& assetPath) {
		if (!std::filesystem::exists(assetPath)) {
			return;
		}

		std::string extension = assetPath.extension().string();

		if (extension == ".meta") {
			return;
		}

		AssetType type = m_assetTypeRegistry.getTypeFromExtension(extension);

		if (type == AssetType::UNKNOWN) {
			Log::warn("Unknown asset type for file: {}", assetPath.string());
			return;
		}

		if (type != AssetType::UNKNOWN) {
			std::filesystem::path metaFilePath = assetPath;
			metaFilePath.replace_extension(assetPath.extension().string() + ".meta");

			auto metadata = m_assetTypeRegistry.createMetadata(type);

			if (std::filesystem::exists(metaFilePath)) {
				Log::info("Parsing existing .meta file: {}", metaFilePath.string());
				metadata->fromJson(jsonUtils::readFromFile(metaFilePath));
			} else {
				Log::info("Creating new .meta file for asset: {}", assetPath.string());
				registerAsset(std::move(metadata), assetPath);
				jsonUtils::saveToFile(metadata->toJson(), metaFilePath);
			}
		}
	}
}