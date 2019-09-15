#include "stdafx.h"
#include "../stb_image.h"

void Icon::Load() {
	TextureUtils::LoadTexture(m_file, 0, [this](const LoadedTexture& data) {
		m_width = data.m_width;
		m_height = data.m_height;

		if (((m_width % 16 + m_height % 16) != 0) || m_width != m_height) {
			LOG_ERROR("[~gIcon~x] Icon at location ~1%s~r is not square or a power of 16", m_file.c_str());
			return;
		}

		m_data = new byte[data.m_size];
		memcpy(m_data, data.m_data, data.m_size);
		m_icon = { (int)m_width, (int)m_height,  m_data };
	});
}

Icon::~Icon() {
	delete[] m_data;
}