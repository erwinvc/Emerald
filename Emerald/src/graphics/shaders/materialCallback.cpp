#include "stdafx.h"

void MaterialCallbackTexture::Callback() {
	m_texture->Bind(m_slot);
}