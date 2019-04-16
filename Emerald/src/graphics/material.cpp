#include "stdafx.h"

Material::Material() :
    m_albedo(GetTextureManager()->GetNullTexture()),
    m_normal(GetTextureManager()->GetNullTexture()),
    m_specular(GetTextureManager()->GetNullTexture()),
    m_emission(GetTextureManager()->GetNullTexture()) {}
