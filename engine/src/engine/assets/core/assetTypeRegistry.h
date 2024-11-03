#pragma once
#include "asset.h"

namespace emerald {
	class Asset;
	class AssetMetadata;

	using AssetFactory = std::function<Ref<Asset>()>;

	class AssetTypeDesc {
	public:
		std::string m_typeName;
		std::vector<std::string> m_extensions;
		AssetFactory m_factory;

		AssetTypeDesc(const std::string& name, std::initializer_list<std::string> exts, AssetFactory fact)
			: m_typeName(name), m_extensions(exts), m_factory(fact) {
		}
	};

	class AssetTypeRegistry {
	public:
		AssetTypeRegistry() = default;

		template<typename T>
		void registerAssetType(const std::string& typeName,
			std::initializer_list<std::string> extensions) {
			AssetTypeDesc desc(
				typeName,
				extensions,
				[]() -> Ref<Asset> { return Ref<T>::create(); }
			);

			m_assetTypes.insert({ typeName , desc });

			for (const auto& ext : extensions) {
				m_extensionMap.insert({ ext, typeName });
			}
		}

		std::string getAssetTypeFromExtension(const std::string& extension) const {
			auto it = m_extensionMap.find(extension);
			return (it != m_extensionMap.end()) ? it->second : "unknown";
		}

		Ref<Asset> createAsset(const std::string& typeName) const {
			auto it = m_assetTypes.find(typeName);
			if (it != m_assetTypes.end()) {
				return it->second.m_factory();
			}
			return nullptr;
		}

		const AssetTypeDesc* getAssetTypeDesc(const std::string& typeName) const {
			auto it = m_assetTypes.find(typeName);
			return (it != m_assetTypes.end()) ? &it->second : nullptr;
			return nullptr;
		}

	private:
		std::unordered_map<std::string, AssetTypeDesc> m_assetTypes;
		std::unordered_map<std::string, std::string> m_extensionMap;
	};
}