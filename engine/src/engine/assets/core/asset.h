#pragma once

namespace emerald {
	enum class AssetType {
		UNKNOWN,
		PREFAB,
		MATERIAL,
		SHADER,
		TEXTURE,
		MODEL,
		AUDIO,
	};

	class Asset : public RefCounted {
	public:
		virtual ~Asset() = default;

		virtual bool load() { return true; };
		virtual bool reload() { return true; };
	};
}