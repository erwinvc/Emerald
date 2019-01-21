#include "stdafx.h"

Material::Material() : m_hasNormal(false), m_albedo(GetTextureManager()->GetNullTexture()), m_normal(GetTextureManager()->GetNullTexture()) {}
