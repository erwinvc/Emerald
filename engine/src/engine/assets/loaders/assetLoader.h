#pragma once
#include "engine/assets/core/asset.h"
#include "core/common/expected.h"

namespace emerald {
	class AssetMetadata;

	class AssetLoader : public RefCounted {
	public:
		AssetLoader() = default;
		virtual ~AssetLoader() = default;

		/**
		 * @brief Can be called from any thread so can be used asynchronously.
		 * Can only be called once.
		 */
		Expected<Empty> beginLoad() {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_state != AssetLoaderState::IDLE) {
				//Log::error("AssetLoader::beginLoad() called in invalid state: {}", getStateName(m_state));
				return Unexpected("AssetLoader::beginLoad() called in invalid state: " + std::string(getStateName(m_state)));
			}
			if (!onBeginLoad()) {
				m_state = AssetLoaderState::FAILED;
				return Unexpected(std::string("AssetLoader::onBeginLoad() failed"));
			}
			m_state = AssetLoaderState::LOADING;
			return Expected<Empty>();
		}

		/**
		 * @brief Must be called from the main thread, finalizes the load after beginLoad() succeeded.
		 * Can only be called once. Returns a loaded asset or nullptr on failure.
		 */
		Expected<Ref<Asset>> finishLoad() {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_state != AssetLoaderState::LOADING) {
				//Log::error("AssetLoader::finishLoad() called in invalid state: {}", getStateName(m_state));
				return Unexpected("AssetLoader::finishLoad() called in invalid state: " + std::string(getStateName(m_state)));
			}
			auto asset = onFinishLoad();
			if (asset) {
				m_state = AssetLoaderState::COMPLETED;
			} else {
				m_state = AssetLoaderState::FAILED;
			}
			return asset;
		}

		Expected<Ref<Asset>> load() {
			auto beginResult = onBeginLoad();
			if (!beginResult) {
				std::string str = beginResult.error();
				return Unexpected(str);
			}
			auto a = onFinishLoad();
			return a;
		}

	protected:
		enum class AssetLoaderState {
			IDLE,
			LOADING,
			COMPLETED,
			FAILED
		};

		/**
		 * @brief Derived classes must implement.
		 * Called once when loading begins (could be async).
		 * Return monostate if successful, or an error message if failed.
		 */
		virtual Expected<Empty> onBeginLoad() = 0;

		/**
		 * @brief Derived classes must implement.
		 * Called once when finalizing the load (main thread).
		 * Return a valid Ref<Asset> if successful, or an error message if failed.
		 */
		virtual Expected<Ref<Asset>> onFinishLoad() = 0;

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