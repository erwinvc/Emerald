#pragma once
#include "engine/assets/metadata/assetMetadata.h"
#include "utils/threading/jobSystem.h"
#include <future>
#include "engine/events/eventSystem.h"

namespace emerald {
	enum class AssetStreamingState {
		NOTLOADED,
		LOADING,
		LOADED
	};

	class Streaming {
	public:
		struct StreamingQueueEntry {
			AssetMetadata* m_metadata;
			std::future<Ref<AssetLoader>> m_job;
		};

		Streaming() = delete;
		Streaming(const Streaming&) = delete;
		Streaming& operator=(const Streaming&) = delete;

		static void streamAsset(AssetMetadata* metadata) {
			if (getAssetStreamingState(metadata) != AssetStreamingState::NOTLOADED) return;

			std::lock_guard<std::mutex> lock(m_mutex);

			Ref<AssetLoader> loader = metadata->createAssetLoader();
			if (loader != nullptr) {
				m_streamingState.insert({ metadata, AssetStreamingState::LOADING });

				std::future<Ref<AssetLoader>> job = JobSystem::submit([metadata, loader] {
					Log::info("Streaming asset: {}", metadata->getPath().string());
					loader->asyncLoad();
					Log::info("Finished streaming asset: {}", metadata->getPath().string());
					return loader;
				});
				m_streamingQueue.push_back(StreamingQueueEntry(metadata, std::move(job)));
			}
		}

		static bool isAssetStreamed(AssetMetadata* metadata) {
			return getAssetStreamingState(metadata) == AssetStreamingState::LOADED;
		}

		static bool isAssetLoading(AssetMetadata* metadata) {
			return getAssetStreamingState(metadata) == AssetStreamingState::LOADING;
		}

		static AssetStreamingState getAssetStreamingState(AssetMetadata* metadata) {
			std::lock_guard<std::mutex> lock(m_mutex);

			auto it = m_streamingState.find(metadata);
			if (it != m_streamingState.end()) {
				return it->second;
			}
			return AssetStreamingState::NOTLOADED;
		}

		static Ref<Asset> getAsset(AssetMetadata* metadata) {
			std::lock_guard<std::mutex> lock(m_mutex);

			auto it = m_assets.find(metadata);
			if (it != m_assets.end()) {
				return it->second;
			}
			return nullptr;
		}

		static void handleStreamingQueue() {
			std::lock_guard<std::mutex> lock(m_mutex);

			for (auto it = m_streamingQueue.begin(); it != m_streamingQueue.end();) {
				if (it->m_job.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
					Ref<AssetLoader> loader = it->m_job.get();

					AssetMetadata* metadata = it->m_metadata;
					m_streamingState[metadata] = AssetStreamingState::LOADED;
					m_assets.insert({ metadata, loader->syncLoad() });

					EventSystem::dispatch<AssetStreamedEvent>(metadata, m_assets[metadata]);

					it = m_streamingQueue.erase(it);
				} else {
					++it;
				}
			}
		}

	private:
		static inline std::mutex m_mutex;
		static inline std::vector<StreamingQueueEntry> m_streamingQueue;
		static inline std::map<AssetMetadata*, AssetStreamingState> m_streamingState;
		static inline std::map<AssetMetadata*, Ref<Asset>> m_assets;
	};
}