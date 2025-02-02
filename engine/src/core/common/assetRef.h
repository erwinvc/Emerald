#pragma once
#include "engine/assets/metadata/assetMetadata.h"
#include "engine/assets/core/assetRegistry.h"
#include "engine/ecs/core/ECSManager.h"

namespace emerald {
	template<typename T>
	class AssetRef {
	public:
		AssetRef() = default;
		explicit AssetRef(const UUID& id) : m_assetID(id) {}

		Ref<T> get() {
			if (auto asset = m_cachedAsset.lock()) {
				return asset;
			}

			if (!m_requested) {
				requestAssetLoad();
			}

			AssetMetadata* assetMetadata = AssetRegistry::getAssetMetadata(m_assetID);
			if (auto asset = AssetRegistry::getAsset(assetMetadata)) {
				m_cachedAsset = asset;
				return m_cachedAsset.lock();
			}

			return nullptr;
		}

		T* operator->() {
			return get();
		}

		operator bool() const {
			return !m_cachedAsset.expired();
		}

		UUID getAssetId() const { return m_assetID; }
		void setAssetId(const UUID& id) {
			m_assetID = id;
			m_cachedAsset.reset();
		}

	private:
		UUID m_assetID;
		WeakRef<T> m_cachedAsset;
		bool m_requested = false;

		void requestAssetLoad() {
			AssetMetadata* assetMetadata = AssetRegistry::getAssetMetadata(m_assetID);
			if (assetMetadata) AssetRegistry::streamAsset(assetMetadata);
			m_requested = true;
		}
	};

	template<typename T>
	class ComponentRef {
	public:
		ComponentRef() = default;
		explicit ComponentRef(const UUID& id) {
			setComponentID(id);
		}

		T* get() {
			if (!ECSManager::ECS().validateComponentID(m_componentID)) {
				m_cachedComponent = nullptr;
			}
			return m_cachedComponent;
		}

		T* operator->() {
			return get();
		}

		operator bool() const {
			return get() != nullptr;
		}

		UUID getComponentID() const { return m_componentID; }
		void setComponentID(const UUID& id) {
			m_componentID = id;
			m_cachedComponent = ECSManager::ECS().getComponentByID<T>(m_componentID);
		}

	private:
		UUID m_componentID;
		T* m_cachedComponent = nullptr;
	};
}