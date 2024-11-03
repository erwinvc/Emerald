#pragma once
#include "utils/uuid/uuid.h"
#include "assetMetadata.h"

namespace emerald {
	class Asset : public RefCounted {
	public:
		virtual bool load() = 0;
		virtual bool reload() = 0;

		const AssetMetadata& getMetaData() const { return m_metaData; }

	private:
		AssetMetadata m_metaData;
	};
}