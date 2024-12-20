#pragma once
#include "engine/assets/core/asset.h"

namespace emerald {
	class AssetMetadata;

	class AssetLoader : public RefCounted {
	public:
		AssetLoader() {}

		virtual void asyncLoad() = 0;
		virtual Ref<Asset> syncLoad() = 0;

		Ref<Asset> load() {
			asyncLoad();
			return syncLoad();
		}
	};
}