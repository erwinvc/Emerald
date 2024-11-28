#pragma once
#include "asset.h"
#include "engine/assets/metadata/assetMetadata.h"

namespace emerald {
	class Asset;

	class AssetTypeRegistry {
	public:
		using MetadataCreator = std::function<UniqueRef<AssetMetadata>()>;
		using ExtensionMap = std::unordered_map<std::string, AssetType>;
		std::unordered_map<AssetType, MetadataCreator> m_creators;

		template<typename T>
		void registerType(AssetType type, const std::vector<std::string>& extensions) {
			m_creators.insert({ type, []() { return UniqueRef<T>::create().as<AssetMetadata>(); } });
			for (const auto& ext : extensions) {
				m_extensionMap.insert({ ext, type });
			}
		}

		UniqueRef<AssetMetadata> createMetadata(AssetType type) const {
			auto it = m_creators.find(type);
			if (it != m_creators.end()) {
				return it->second();
			}
			return nullptr;
		}

		AssetType getTypeFromExtension(const std::string& extension) const {
			auto it = m_extensionMap.find(extension);
			if (it != m_extensionMap.end()) {
				return it->second;
			}
			return AssetType::UNKNOWN;
		}

	private:
		ExtensionMap m_extensionMap;
	};
}