#pragma once
#include "core/common/ref.h"

namespace emerald {
	enum class AssetType {
		DEFAULT,
		FOLDER,
		PREFAB,
		MATERIAL,
		SHADER,
		TEXTURE,
		SCENE,
		MODEL,
		AUDIO,
		SCRIPT,
		FONT
	};

	inline const char* assetTypeToString(AssetType type, bool capitalizeFirstLetter = false) {
		switch (type) {
			case AssetType::DEFAULT: return capitalizeFirstLetter ? "Default" : "default";
			case AssetType::FOLDER:  return capitalizeFirstLetter ? "Folder" : "folder";
			case AssetType::PREFAB:  return capitalizeFirstLetter ? "Prefab" : "prefab";
			case AssetType::MATERIAL:return capitalizeFirstLetter ? "Material" : "material";
			case AssetType::SHADER:  return capitalizeFirstLetter ? "Shader" : "shader";
			case AssetType::TEXTURE: return capitalizeFirstLetter ? "Texture" : "texture";
			case AssetType::MODEL:   return capitalizeFirstLetter ? "Model" : "model";
			case AssetType::AUDIO:   return capitalizeFirstLetter ? "Audio" : "audio";
			case AssetType::SCRIPT:  return capitalizeFirstLetter ? "Script" : "script";
			case AssetType::FONT:    return capitalizeFirstLetter ? "Font" : "font";
		}
		throw std::runtime_error("Unknown asset type");
	}

	class Asset : public RefCounted {
	public:
		virtual ~Asset() = default;

		virtual bool load() { return true; };
		virtual bool reload() { return true; };
	};
}