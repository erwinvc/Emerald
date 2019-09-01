#include "stdafx.h"
#include "../stb_image.h"

void Icon::Load() {
	TextureUtils::LoadTexture(m_file, 0, [this](byte* data, uint width, uint height) {
		m_width = width;
		m_height = height;

		if (((m_width % 16 + m_height % 16) != 0) || m_width != m_height) {
			LOG_ERROR("[~gIcon~x] Icon at location ~1%s~r is not square or a power of 16", m_file.c_str());
			return;
		}
		int size = 4 * m_width * m_height;
		m_data = new byte[size];
		memcpy(m_data, data, size);
		m_icon = { (int)m_width, (int)m_height,  m_data };
	});
}

Icon::~Icon() {
	delete[] m_data;
}