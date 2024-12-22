#pragma once
#include "engine/assets/core/asset.h"

namespace emerald {
	class AssetMetadata;

	class AssetLoader : public RefCounted {
	public:
		AssetLoader() = default;
		virtual ~AssetLoader() = default;

		bool beginLoad() {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_state != AssetLoaderState::IDLE) {
				Log::error("AssetLoader::beginLoad() called in invalid state: {}", getStateName(m_state));
				return false;
			}
			if (!onBeginLoad()) {
				m_state = AssetLoaderState::FAILED;
				return false;
			}
			m_state = AssetLoaderState::LOADING;
			return true;
		}

		/**
		 * @brief Must be called from the main thread, finalizes the load after beginLoad() succeeded.
		 * Can only be called once. Returns a loaded asset or nullptr on failure.
		 */
		Ref<Asset> finishLoad() {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_state != AssetLoaderState::LOADING) {
				Log::error("AssetLoader::finishLoad() called in invalid state: {}", getStateName(m_state));
				return nullptr;
			}
			auto asset = onFinishLoad();
			if (asset) {
				m_state = AssetLoaderState::COMPLETED;
			} else {
				m_state = AssetLoaderState::FAILED;
			}
			return asset;
		}

		Ref<Asset> load() {
			if (onBeginLoad()) {
				return onFinishLoad();
			}
			return nullptr;
		}

	private:
		enum class AssetLoaderState {
			IDLE,
			LOADING,
			COMPLETED,
			FAILED
		};

		virtual bool onBeginLoad() = 0;
		virtual Ref<Asset> onFinishLoad() = 0;

		AssetLoaderState m_state = AssetLoaderState::IDLE;
		mutable std::mutex m_mutex;

		const char* getStateName(AssetLoaderState s) const {
			switch (s) {
				case AssetLoaderState::IDLE:      return "idle";
				case AssetLoaderState::LOADING:   return "loading";
				case AssetLoaderState::COMPLETED: return "completed";
				case AssetLoaderState::FAILED:    return "failed";
			}
			return "Unknown";
		}
	};
}