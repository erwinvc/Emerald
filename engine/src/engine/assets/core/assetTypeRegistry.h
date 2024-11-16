#pragma once
#include "asset.h"

namespace emerald {
	class Asset;
	class AssetMetadata;

	using AssetFactory = std::function<Ref<Asset>()>;

	class AssetTypeDesc {
	public:
		AssetType m_type;
		std::vector<std::string> m_extensions;
		AssetFactory m_factory;

		AssetTypeDesc(AssetType type, std::initializer_list<std::string> exts, AssetFactory fact)
			: m_type(type), m_extensions(exts), m_factory(fact) {
		}
	};

	class AssetTypeRegistry {
	public:
		AssetTypeRegistry() = default;

		template<typename T>
		void registerAssetType(AssetType type,std::initializer_list<std::string> extensions) {
			AssetTypeDesc desc(type, extensions, []() -> Ref<Asset> { return Ref<T>::create(); });

			m_assetTypes.insert({ type , desc });

			for (const auto& ext : extensions) {
				m_extensionMap.insert({ ext, type });
			}
		}

		AssetType getAssetTypeFromExtension(const std::string& extension) const {
			auto it = m_extensionMap.find(extension);
			return (it != m_extensionMap.end()) ? it->second : AssetType::UNKNOWN;
		}

		Ref<Asset> createAsset(AssetType type) const {
			auto it = m_assetTypes.find(type);
			if (it != m_assetTypes.end()) {
				return it->second.m_factory();
			}
			return nullptr;
		}

		const AssetTypeDesc* getAssetTypeDesc(AssetType type) const {
			auto it = m_assetTypes.find(type);
			return (it != m_assetTypes.end()) ? &it->second : nullptr;
			return nullptr;
		}

		const AssetTypeDesc* getAssetTypeDesc(const std::string& extension) const {
			AssetType type = getAssetTypeFromExtension(extension);
			auto it = m_assetTypes.find(type);
			return (it != m_assetTypes.end()) ? &it->second : nullptr;
			return nullptr;
		}

	private:
		std::unordered_map<AssetType, AssetTypeDesc> m_assetTypes;
		std::unordered_map<std::string, AssetType> m_extensionMap;
	};
}