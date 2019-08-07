#include "stdafx.h"
#include "../stb_image.h"

void Icon::Load() {
	if (!FileSystem::DoesFileExist(m_file)) {
		LOG_ERROR("[~gIcon~x] file at ~1%s~x does not exist!", m_file.c_str());
		return;
	}

	int bpc;

	stbi_set_flip_vertically_on_load(0);
	byte* data = stbi_load(m_file.c_str(), (int*)&m_width, (int*)&m_height, &bpc, 4);

	if (((m_width % 16 + m_height % 16) != 0) || m_width != m_height) {
		LOG_ERROR("[~gIcon~x] Icon at location ~1~r is not square or a power of 16", bpc, m_file.c_str());
		return;
	}

	if (data) {
		m_data = data;
		m_icon = { (int)m_width, (int)m_height,  m_data};
		LOG("[~gIcon~x] Loaded ~1%s", m_file.c_str());
	} else LOG_ERROR("[~gIcon~x] Icon to load ~1%s", m_file.c_str());

}

Icon::~Icon() {
	stbi_image_free(m_data);
}