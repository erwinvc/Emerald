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
		if (state == AssetStreamingState::LOADED || state == AssetStreamingState::LOADING || state == AssetStreamingState::CANNOTLOAD) {
			return;
		}

		std::lock_guard<std::mutex> lock(m_mutex);

		m_streamingState[metadata] = AssetStreamingState::LOADING;
		startLoading(metadata);
	}

	void AssetRegistry::startLoading(AssetMetadata* metadata) {
		m_streamingQueue.emplace_back(metadata, Ref<AssetLoader>());

		StreamingTask& task = m_streamingQueue.back();
		JobSystem::execute(task.m_ctx, [&task](JobArgs args) {
			Log::info("Loading asset from metadata: {}", task.m_metadata->getPath().string());

			Ref<AssetLoader> loader = task.m_metadata->createAssetLoader();
			if (!loader) {
				Log::error("Failed to create asset loader instance for {}", task.m_metadata->getPath().string());
				m_streamingState[task.m_metadata] = AssetStreamingState::CANNOTLOAD;
				return;
			}

			if (!loader->beginLoad()) {
				Log::error("Failed async loading {}", task.m_metadata->getPath().string());
				return;
			}

			task.m_loader = loader;
			Log::info("Finished async loading {}", task.m_metadata->getPath().string());
			});

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
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_uuidAssetMap.find(uuid);
		if (it != m_uuidAssetMap.end()) {
			return it->second;
		}
		return nullptr;
	}

	AssetMetadata* AssetRegistry::getAssetMetadata(const std::filesystem::path& path) {
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_pathAssetMap.find(path);
		if (it != m_pathAssetMap.end()) {
			return it->second;
		}
		return nullptr;
	}

	void AssetRegistry::clear() {
		std::lock_guard<std::mutex> lock(m_mutex);

		m_assets.clear();
		m_uuidAssetMap.clear();
		m_pathAssetMap.clear();
	}

	void AssetRegistry::update() {
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto it = m_streamingQueue.begin(); it != m_streamingQueue.end();) {
			StreamingTask& task = *it;
			if (!JobSystem::isBusy(task.m_ctx)) {
				finalizeLoading(task.m_metadata, task.m_loader);
				it = m_streamingQueue.erase(it);
			} else {
				++it;
			}
		}
	}

	void AssetRegistry::finalizeLoading(AssetMetadata* metadata, const Ref<AssetLoader>& loader) {
		if (m_streamingState[metadata] == AssetStreamingState::CANNOTLOAD) return;
		if (!loader) {
			Log::warn("Asset {} failed to load", metadata->getPath().string());
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
		Log::info("Asset {} is loaded", metadata->getPath().string());
	}
}