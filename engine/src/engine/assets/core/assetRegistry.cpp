#include "eepch.h"
#include "assetRegistry.h"
#include "../texture/textureAsset.h"
#include "../../editor/src/project.h"
#include "utils/uuid/uuidGenerator.h"
#include "../metadata/textureMetadata.h"
#include "../model/modelMetadata.h"

namespace emerald {
	AssetRegistry::AssetRegistry() {
		m_assetTypeRegistry.registerType<TextureMetadata>(AssetType::TEXTURE, { ".png", ".bmp", ".jpg", ".jpeg" });
		m_assetTypeRegistry.registerType<ModelMetadata>(AssetType::MODEL, { ".fbx", ".obj" });
	}

	void AssetRegistry::parseCurrentProject() {
		clear();
		auto a = Project::GetAssetsPath();
		for (const auto& entry : std::filesystem::recursive_directory_iterator(a)) {
			if (entry.path().extension() != ".meta") {
				processAssetFile(entry.path());
			}
		}
	}

	void AssetRegistry::registerAsset(UniqueRef<AssetMetadata> metadata, const std::filesystem::path& path) {
		m_assets.push_back(std::move(metadata));

		auto inserted = m_assets.back().raw();
		m_uuidAssetMap[inserted->getUUID()] = inserted;
		m_pathAssetMap[path] = inserted;
	}

	void AssetRegistry::processAssetFile(const std::filesystem::path& path) {
		if (!std::filesystem::exists(path)) {
			return;
		}

		if (path.extension() == ".meta") {
			return;
		}

		AssetType type = m_assetTypeRegistry.getAssetType(path);

		std::filesystem::path metaFilePath = path;
		metaFilePath.replace_extension(path.extension().string() + ".meta");

		auto metadata = m_assetTypeRegistry.createMetadata(path, type);
		if (!metadata) {
			Log::fatal("Failed to create metadata for asset: {}", path.string());
			return;
		}
		if (std::filesystem::exists(metaFilePath)) {
			Log::info("Parsing existing .meta file: {}", metaFilePath.string());
			metadata->fromJson(jsonUtils::readFromFile(metaFilePath));
		} else {
			Log::info("Creating new .meta file for asset: {}", path.string());
			jsonUtils::saveToFile(metadata->toJson(), metaFilePath);
		}
		registerAsset(std::move(metadata), path);
	}
}