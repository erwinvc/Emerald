#pragma once
#include "asset.h"
#include "assetTypeRegistry.h"
#include "engine/assets/metadata/assetMetadata.h"
#include "utils/uuid/uuid.h"
#include "../loaders/assetLoader.h"
#include "utils/threading/jobSystem.h"
#include "../../events/fileChangedEvent.h"
#include "utils/datastructures/vector.h"

namespace emerald {
	class AssetRegistry {
	public:
		enum class AssetStreamingState {
			NOTLOADED,
			LOADING,
			LOADED,
			CANNOTLOAD
		};

		AssetRegistry() = delete;
		AssetRegistry(const AssetRegistry&) = delete;
		AssetRegistry& operator=(const AssetRegistry&) = delete;

		static void initialize();
		static void parseCurrentProject();
		static void processAssetFile(const std::filesystem::path& assetPath);
		static void removeAsset(const std::filesystem::path& assetPath);
		static void removeOrphanedMetaFile(const std::filesystem::path& path);
		static void streamAsset(AssetMetadata* metadata);

		static bool isAssetStreamed(AssetMetadata* metadata) {
			return getAssetStreamingState(metadata) == AssetStreamingState::LOADED;
		}

		static bool isAssetLoading(AssetMetadata* metadata) {
			return getAssetStreamingState(metadata) == AssetStreamingState::LOADING;
		}

		static AssetStreamingState getAssetStreamingState(AssetMetadata* metadata);
		static Ref<Asset> getAsset(AssetMetadata* metadata);
		static AssetMetadata* getAssetMetadata(const UUID& uuid);
		static AssetMetadata* getAssetMetadata(const std::filesystem::path& path);
		static bool executeWhenAssetStreamed(AssetMetadata* metadata, std::function<void(const Ref<Asset>&)> callback);

		static void clear();
		static void update();

		private:
			struct StreamingTask {
				Context m_ctx;
				AssetMetadata* m_metadata;
				Ref<AssetLoader> m_loader;
				Expected<Empty> m_beginLoadResult;

				StreamingTask(AssetMetadata* metadata, Ref<AssetLoader>&& loader)
					: m_ctx(), m_metadata(metadata), m_loader(loader) {}
			};

		static void startLoading(AssetMetadata* metadata);
		static void finalizeLoading(StreamingTask& task);
		static void onFileChangedEvent(FileChangedEvent& e);

		static inline AssetTypeRegistry m_assetTypeRegistry;
		static inline Vector<UniqueRef<AssetMetadata>> m_assetMetadata;
		static inline std::unordered_map<UUID, AssetMetadata*> m_uuidAssetMap;
		static inline std::unordered_map<std::filesystem::path, AssetMetadata*> m_pathAssetMap;

		static inline std::mutex m_mutex;
		static inline std::list<StreamingTask> m_streamingQueue;
		static inline std::map<AssetMetadata*, AssetStreamingState> m_streamingState;
		static inline std::map<AssetMetadata*, Ref<Asset>> m_assets;
	};
}