#include "eepch.h"
#include "assetRegistry.h"
#include "utils/threading/jobSystem.h"
#include "engine/assets/metadata/textureMetadata.h"
#include "engine/assets/model/modelMetadata.h"
#include "../../events/eventSystem.h"
#include "../../events/fileChangedEvent.h"
#include "core/application/application.h"
#include "../metadata/sceneMetadata.h"
#include "../../editor/src/core/projectManager.h"
#include "utils/system/fileSystem.h"

namespace emerald {
	void AssetRegistry::initialize() {
		m_assetTypeRegistry.registerType<TextureMetadata>(AssetType::TEXTURE, { ".png", ".bmp", ".jpg", ".jpeg" });
		m_assetTypeRegistry.registerType<ModelMetadata>(AssetType::MODEL, { ".fbx", ".obj", ".gltf" });
		m_assetTypeRegistry.registerType<SceneMetadata>(AssetType::SCENE, { ".scene" });

		EventSystem::subscribe<FileChangedEvent>(onFileChangedEvent);
	}

	void AssetRegistry::parseCurrentProject() {
		clear();
		auto assetsPath = ProjectManager::getCurrentProject().getAssetsFolder();
		for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
			processAssetFile(entry.path());
		}
	}

	void AssetRegistry::processAssetFile(const std::filesystem::path& path) {
		if (!std::filesystem::exists(path)) {
			removeOrphanedMetaFile(path);
			return;
		}

		if (path.extension() == ".meta") {
			// Check if the associated asset file exists
			std::filesystem::path assetPath = path;
			std::string metaExt = ".meta";
			std::string pathStr = FileSystem::pathToString(assetPath);
			if (pathStr.length() > metaExt.length()) {
				// Remove .meta extension
				pathStr.erase(pathStr.length() - metaExt.length());
				assetPath = pathStr;

				if (!std::filesystem::exists(assetPath)) {
					// The asset file doesn't exist, so this is an orphaned .meta file
					try {
						std::filesystem::remove(path);
						Log::info("[Assets] Removed orphaned meta file: {}", path.u8string());
					} catch (const std::exception& e) {
						Log::warn("[Assets] Failed to remove orphaned meta file {}: {}", path.u8string(), e.what());
					}
				}
			}
			return;
		}

		AssetType type = m_assetTypeRegistry.getAssetType(path);

		std::filesystem::path metaFilePath = path;
		metaFilePath.replace_extension(path.extension().wstring() + L".meta");

		auto metadata = m_assetTypeRegistry.createMetadata(path, type);
		if (!metadata) {
			Log::fatal("[Assets] Failed to create metadata for asset: {}", path.u8string());
			return;
		}

		if (std::filesystem::exists(metaFilePath)) {
			try {
				auto j = jsonUtils::readFromFile(metaFilePath);
				metadata->fromJson(j);
			} catch (const std::exception& e) {
				Log::warn("[Assets] Error parsing .meta file {}: {}", metaFilePath.u8string(), e.what());
				jsonUtils::saveToFile(metadata->toJson(), metaFilePath);
			}
		} else {
			try {
				jsonUtils::saveToFile(metadata->toJson(), metaFilePath);
			} catch (const std::exception& e) {
				Log::warn("[Assets] Could not save .meta file for {}: {}", path.u8string(), e.what());
			}
		}

		AssetMetadata* rawPtr = metadata.raw();

		m_assetMetadata.pushBack(std::move(metadata));

		m_uuidAssetMap[rawPtr->getUUID()] = rawPtr;
		m_pathAssetMap[path] = rawPtr;

		std::lock_guard<std::mutex> lock(m_mutex);
		m_streamingState[rawPtr] = AssetStreamingState::NOTLOADED;
	}

	void AssetRegistry::removeAsset(const std::filesystem::path& path) {
		if (path.extension() == L".meta") {
			auto realAssetPath = path.parent_path() / path.stem();
			if (std::filesystem::exists(realAssetPath)) {
				AssetType type = m_assetTypeRegistry.getAssetType(realAssetPath);

				std::filesystem::path metaFilePath = realAssetPath;
				metaFilePath.replace_extension(metaFilePath.extension().wstring() + L".meta");
				auto metadata = m_assetTypeRegistry.createMetadata(realAssetPath, type);
				if (metadata) {
					jsonUtils::saveToFile(metadata->toJson(), metaFilePath);
				}
			}
			return;
		}

		removeOrphanedMetaFile(path);

		AssetMetadata* metadata = getAssetMetadata(path);
		if (!metadata) {
			return;
		}

		m_assetMetadata.removeIf([metadata](const auto& a) {return a.raw() == metadata; });

		m_uuidAssetMap[metadata->getUUID()] = nullptr;
		m_pathAssetMap[path] = nullptr;
	}

	void AssetRegistry::removeOrphanedMetaFile(const std::filesystem::path& path) {
		std::filesystem::path possibleMetaPath = path;
		possibleMetaPath.replace_extension(path.extension().wstring() + L".meta");
		if (std::filesystem::exists(possibleMetaPath)) {
			try {
				std::filesystem::remove(possibleMetaPath);
				Log::info("[Assets] Removed orphaned meta file: {}", possibleMetaPath.u8string());
			} catch (const std::exception& e) {
				Log::warn("[Assets] Failed to remove orphaned meta file {}: {}", possibleMetaPath.u8string(), e.what());
			}
		}
	}
	void AssetRegistry::streamAsset(AssetMetadata* metadata) {
		ASSERT(metadata, "[Assets] Cannot stream null metadata")
			AssetStreamingState state = getAssetStreamingState(metadata);
		if (state == AssetStreamingState::LOADED || state == AssetStreamingState::LOADING || state == AssetStreamingState::CANNOTLOAD) {
			return;
		}

		std::lock_guard<std::mutex> lock(m_mutex);

		m_streamingState[metadata] = AssetStreamingState::LOADING;
		startLoading(metadata);
	}

	void AssetRegistry::startLoading(AssetMetadata* metadata) {

		Ref<AssetLoader> assetLoader = metadata->createAssetLoader();
		if (!assetLoader) {
			//Log::error("Failed to create asset loader instance for {}", task.m_metadata->getPath().u8string());
			m_streamingState[metadata] = AssetStreamingState::CANNOTLOAD;
			return;
		}
		m_streamingQueue.emplace_back(metadata, Ref<AssetLoader>());
		StreamingTask& task = m_streamingQueue.back();
		task.m_loader = assetLoader;

		JobSystem::execute(task.m_ctx, [&task](JobArgs args) {
			auto result = task.m_loader->beginLoad();
			if (result) {
				task.m_beginLoadResult = Expected<Empty>();
			} else {
				task.m_beginLoadResult = Unexpected(result.error());
			}
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

	bool AssetRegistry::executeWhenAssetStreamed(AssetMetadata* metadata, std::function<void(const Ref<Asset>&)> callback) {
		if (isAssetStreamed(metadata)) {
			callback(getAsset(metadata));
			return true;
		} else {
			streamAsset(metadata);
			EventSystem::subscribeOnce<AssetStreamedEvent>([metadata, callback](AssetStreamedEvent& e) {
				if (e.getMetadata() == metadata) {
					callback(e.getAsset());
					e.setHandled();
				}
			});
			return false;
		}
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
			if (!task.m_ctx.isBusy()) {
				finalizeLoading(task);
				it = m_streamingQueue.erase(it);
			} else {
				++it;
			}
		}
	}

	void AssetRegistry::finalizeLoading(StreamingTask& task) {
		//if (m_streamingState[metadata] == AssetStreamingState::CANNOTLOAD) return;

		if (task.m_beginLoadResult.hasValue()) {
			auto result = task.m_loader->finishLoad();

			if (result.hasValue()) {
				m_assets[task.m_metadata] = result.value();
				m_streamingState[task.m_metadata] = AssetStreamingState::LOADED;
			} else {
				m_streamingState[task.m_metadata] = AssetStreamingState::NOTLOADED;
			}
			EventSystem::dispatch<AssetStreamedEvent>(task.m_metadata, m_assets[task.m_metadata]);
			Log::info("[Assets] loaded Asset: {}", task.m_metadata->getPath().u8string());
		} else {
			Log::error("[Assets] Failed to load asset: {}", task.m_beginLoadResult.error());
			m_streamingState[task.m_metadata] = AssetStreamingState::CANNOTLOAD;
		}
	}

	void AssetRegistry::onFileChangedEvent(FileChangedEvent& e) {
		switch (e.getType()) {
			case FileChangedEventType::DELETED:
				removeAsset(e.getPath());
				break;
			case FileChangedEventType::CREATED:
			case FileChangedEventType::MODIFIED:
				processAssetFile(e.getPath());
				break;
		}

		Log::info("[Assets] File changed: {} {} {}", e.getPath().u8string(), (uint32_t)e.getType(), App->getFrameCount());
	}
}