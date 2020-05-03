#include "stdafx.h"

uint Material::s_boundMaterial = 0xFFFFFFFFu;

void Material::Bind() {
	m_shader->Bind();
	if (s_boundMaterial == m_ID) {
		//Application::s_globalLogValue++;
		return;
	}
	for (auto& member : m_members) {
		member->Set(m_shader);
	}
	s_boundMaterial = m_ID;
}

Material::Material(const String& name, Shader* shader, uint ID) : m_name(name), m_shader(shader), m_ID(ID) {
	const ShaderProperties* properties = shader->GetShaderProperties();
	for (auto& prop : properties->GetProperties()) {
		switch (prop->m_type) {
			case ShaderPropertyType::RGB: {
				MaterialMemberRGB* member = NEW(MaterialMemberRGB());
				member->m_value = ((ShaderPropertyRGB*)prop)->m_default;
				member->m_type = prop->m_type;
				member->m_name = prop->m_name;
				member->m_uniform = prop->m_uniform;
				m_members.push_back(member);
			} break;
			case ShaderPropertyType::RGBA: {
				MaterialMemberRGBA* member = NEW(MaterialMemberRGBA());
				member->m_value = ((ShaderPropertyRGBA*)prop)->m_default;
				member->m_type = prop->m_type;
				member->m_name = prop->m_name;
				member->m_uniform = prop->m_uniform;
				m_members.push_back(member);
			} break;
			case ShaderPropertyType::FLOAT: {
				MaterialMemberFloat* member = NEW(MaterialMemberFloat());
				member->m_value = ((ShaderPropertyFloat*)prop)->m_default;
				member->m_type = prop->m_type;
				member->m_name = prop->m_name;
				member->m_uniform = prop->m_uniform;
				m_members.push_back(member);
			} break;
			case ShaderPropertyType::RANGE: {
				MaterialMemberRange* member = NEW(MaterialMemberRange());
				member->m_value = ((ShaderPropertyRange*)prop)->m_default;
				member->m_min = ((ShaderPropertyRange*)prop)->m_min;
				member->m_max = ((ShaderPropertyRange*)prop)->m_max;
				member->m_type = prop->m_type;
				member->m_name = prop->m_name;
				member->m_uniform = prop->m_uniform;
				m_members.push_back(member);
			} break;
			case ShaderPropertyType::INT: {
				MaterialMemberInt* member = NEW(MaterialMemberInt());
				member->m_value = ((ShaderPropertyInt*)prop)->m_default;
				member->m_type = prop->m_type;
				member->m_name = prop->m_name;
				member->m_uniform = prop->m_uniform;
				m_members.push_back(member);
			} break;
			case ShaderPropertyType::BOOL: {
				MaterialMemberBool* member = NEW(MaterialMemberBool());
				member->m_value = ((ShaderPropertyBool*)prop)->m_default;
				member->m_type = prop->m_type;
				member->m_name = prop->m_name;
				member->m_uniform = prop->m_uniform;
				m_members.push_back(member);
			} break;
			case ShaderPropertyType::VECTOR2: {
				MaterialMemberVec2* member = NEW(MaterialMemberVec2());
				member->m_value = ((ShaderPropertyVec2*)prop)->m_default;
				member->m_type = prop->m_type;
				member->m_name = prop->m_name;
				member->m_uniform = prop->m_uniform;
				m_members.push_back(member);
			} break;
			case ShaderPropertyType::VECTOR3: {
				MaterialMemberVec3* member = NEW(MaterialMemberVec3());
				member->m_value = ((ShaderPropertyVec3*)prop)->m_default;
				member->m_type = prop->m_type;
				member->m_name = prop->m_name;
				member->m_uniform = prop->m_uniform;
				m_members.push_back(member);
			} break;
			case ShaderPropertyType::VECTOR4: {
				MaterialMemberVec4* member = NEW(MaterialMemberVec4());
				member->m_value = ((ShaderPropertyVec4*)prop)->m_default;
				member->m_type = prop->m_type;
				member->m_name = prop->m_name;
				member->m_uniform = prop->m_uniform;
				m_members.push_back(member);
			} break;
			case ShaderPropertyType::TEXTURE: {
				MaterialMemberTexture* member = NEW(MaterialMemberTexture());
				member->m_slot = ((ShaderPropertyTexture*)prop)->m_slot;
				member->m_textureType = ((ShaderPropertyTexture*)prop)->m_default;
				member->m_type = prop->m_type;
				member->m_name = prop->m_name;
				member->m_uniform = prop->m_uniform;
				switch (((ShaderPropertyTexture*)prop)->m_default) {
					case ShaderPropertyTextureType::INVALID:
						member->m_texture = GetTextureManager()->GetNullTexture();
						break;
					case ShaderPropertyTextureType::WHITE:
						member->m_texture = GetTextureManager()->GetWhiteTexture();
						break;
					case ShaderPropertyTextureType::BLACK:
						member->m_texture = GetTextureManager()->GetBlackTexture();
						break;
					case ShaderPropertyTextureType::NORMAL:
						member->m_texture = GetTextureManager()->GetNormalTexture();
						break;
					case ShaderPropertyTextureType::TRANSPARENT:
						member->m_texture = GetTextureManager()->GetTransparentTexture();
						break;
				}
				m_members.push_back(member);
			} break;
		}
	}
	for (auto& member : m_members) member->GetUniformLocation(m_shader);
}