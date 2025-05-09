#pragma once
#include "asset.h"
#include "engine/assets/metadata/assetMetadata.h"
#include "../metadata/folderMetadata.h"
#include "../metadata/defaultMetadata.h"
#include "utils/system/fileSystem.h"

namespace emerald {
	class Asset;

	class AssetTypeRegistry {
	public:
		using MetadataCreator = std::function<UniqueRef<AssetMetadata>(const std::filesystem::path&)>;
		std::unordered_map<AssetType, MetadataCreator> m_creators;

		template<typename T>
		void registerType(AssetType type, const std::vector<std::string>& extensions) {
			m_creators.insert({ type, [type](const std::filesystem::path& path) {
				return createMetadataFromType<T>(path, type);
			} });

			for (const auto& ext : extensions) {
				m_extensionMap.insert({ ext, type });
			}
		}

		UniqueRef<AssetMetadata> createMetadata(const std::filesystem::path& path, AssetType type) const {
			if (type == AssetType::DEFAULT) {
				return createMetadataFromType<DefaultMetadata>(path, type);
			} else if (type == AssetType::FOLDER) {
				return createMetadataFromType<FolderMetadata>(path, type);
			}

			auto it = m_creators.find(type);
			if (it != m_creators.end()) {
				return it->second(path);
			}
			return nullptr;
		}

		AssetType getAssetType(const std::filesystem::path& path) const {
			if (std::filesystem::is_directory(path)) return AssetType::FOLDER;
			std::string extension = FileSystem::pathToString(path.extension());
			auto it = m_extensionMap.find(extension);
			if (it != m_extensionMap.end()) {
				return it->second;
			}
			return AssetType::DEFAULT;
		}

	private:
		std::unordered_map<std::string, AssetType> m_extensionMap;

		template<typename T>
		static UniqueRef<AssetMetadata> createMetadataFromType(const std::filesystem::path& path, AssetType type) {
			auto instance = UniqueRef<T>::create(path);
			auto instanceAsAssetMetadata = instance.as<AssetMetadata>();
			if (instanceAsAssetMetadata == nullptr) {
				Log::fatal("Metadata factory for {} asset type does not inherit from AssetMetadata", assetTypeToString(type));
			}
			return instanceAsAssetMetadata;
		}
	};
}