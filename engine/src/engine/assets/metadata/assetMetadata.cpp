#include "eepch.h"
#include "assetMetadata.h"
#include "utils/uuid/uuidGenerator.h"

namespace emerald {
	AssetMetadata::AssetMetadata(AssetType type, const std::filesystem::path& path) : m_uuid(UUIDGenerator::create()), m_type(type), m_path(path) {
	}
}