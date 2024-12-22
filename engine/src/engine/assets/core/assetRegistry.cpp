#include "eepch.h"
#include "assetRegistry.h"
#include "../texture/textureAsset.h"
#include "../../editor/src/project.h"
#include "utils/uuid/uuidGenerator.h"
#include "../metadata/textureMetadata.h"
#include "../model/modelMetadata.h"
#include <future>
#include "utils/threading/jobSystem.h"
#include "../../events/eventSystem.h"

namespace emerald {
	void AssetRegistry::initialize() {
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
			try {
				auto j = jsonUtils::readFromFile(metaFilePath);
				metadata->fromJson(j);
			} catch (const std::exception& e) {
				Log::warn("Error parsing .meta file {}: {}", metaFilePath.string(), e.what());
				jsonUtils::saveToFile(metadata->toJson(), metaFilePath);
			}
		} else {
			try {
				jsonUtils::saveToFile(metadata->toJson(), metaFilePath);
			} catch (const std::exception& e) {
				Log::warn("Could not save .meta file for {}: {}", path.string(), e.what());
			}
		}

		AssetMetadata* rawPtr = metadata.raw();

		m_assetMetadata.push_back(std::move(metadata));

		m_uuidAssetMap[rawPtr->getUUID()] = rawPtr;
		m_pathAssetMap[path] = rawPtr;

		std::lock_guard<std::mutex> lock(m_mutex);
		m_streamingState[rawPtr] = AssetStreamingState::NOTLOADED;
	}

	void AssetRegistry::streamAsset(AssetMetadata* metadata) {
		AssetStreamingState state = getAssetStreamingState(metadata);
		if (state == AssetStreamingState::LOADED || state == AssetStreamingState::LOADING) {
			return;
		}

		std::lock_guard<std::mutex> lock(m_mutex);

		m_streamingState[metadata] = AssetStreamingState::LOADING;
		startLoading(metadata);
	}

	void AssetRegistry::startLoading(AssetMetadata* metadata) {
		std::future<Ref<AssetLoader>> fut = JobSystem::submit([metadata]() -> Ref<AssetLoader> {
			Log::info("Loading asset from metadata: {}", metadata->getPath().string());

			Ref<AssetLoader> loader = metadata->createAssetLoader();
			if (!loader) {
				Log::error("Failed to create asset loader instance for {}", metadata->getPath().string());
				return nullptr;
			}

			if (!loader->beginLoad()) {
				Log::error("Failed async loading {}", metadata->getPath().string());
				return nullptr;
			}

			Log::info("Finished async loading {}", metadata->getPath().string());
			return loader;
		});

		m_streamingQueue.push_back({ metadata, std::move(fut) });
	}

	AssetRegistry::AssetStreamingState AssetRegistry::getAssetStreamingState(AssetMetadata* metadata) {
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_streamingState.find(metadata);
		if (it != m_streamingState.end()) {
			return it->second;
		}
		return AssetStreamingState::NOTLOADED;
	}

	Ref<Asset> AssetRegistry::getAsset(AssetMetadata* metadata) {
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_assets.find(metadata);
		if (it != m_assets.end()) {
			return it->second;
		}
		return nullptr;
	}

	AssetMetadata* AssetRegistry::getAssetMetadata(const UUID& uuid) {
		auto it = m_uuidAssetMap.find(uuid);
		if (it != m_uuidAssetMap.end()) {
			return it->second;
		}
		return nullptr;
	}

	AssetMetadata* AssetRegistry::getAssetMetadata(const std::filesystem::path& path) {
		auto it = m_pathAssetMap.find(path);
		if (it != m_pathAssetMap.end()) {
			return it->second;
		}
		return nullptr;
	}

	void AssetRegistry::clear() {
		m_assets.clear();
		m_uuidAssetMap.clear();
		m_pathAssetMap.clear();
	}

	void AssetRegistry::update() {
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto it = m_streamingQueue.begin(); it != m_streamingQueue.end();) {
			StreamingTask& task = *it;
			std::future_status status = task.futureAsset.wait_for(std::chrono::seconds(0));
			if (status == std::future_status::ready) {
				Ref<AssetLoader> loader = task.futureAsset.get();
				Log::info("Asset {} is loaded.", task.metadata->getPath().string());
				finalizeLoading(task.metadata, loader);
				it = m_streamingQueue.erase(it);
			} else {
				++it;
			}
		}
	}

	void AssetRegistry::finalizeLoading(AssetMetadata* metadata, const Ref<AssetLoader>& loader) {
		if (!loader) {
			Log::warn("Asset {} failed to load properly.", metadata->getPath().string());
			m_streamingState[metadata] = AssetStreamingState::NOTLOADED;
			return;
		}

		Ref<Asset> asset = loader->finishLoad();
		if (asset) {
			m_assets[metadata] = asset;
			m_streamingState[metadata] = AssetStreamingState::LOADED;
		} else {
			m_streamingState[metadata] = AssetStreamingState::NOTLOADED;
		}
		EventSystem::dispatch<AssetStreamedEvent>(metadata, m_assets[metadata]);
		Log::info("Asset {} is loaded.", metadata->getPath().string());
	}
}