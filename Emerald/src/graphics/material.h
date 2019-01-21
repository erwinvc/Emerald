#pragma once

class Texture;

class Material {
private:
    bool m_hasNormal;
    Texture* m_albedo;
    Texture* m_normal;
public:
    Material();
    Material(Texture* albedo, Texture* normal) : m_hasNormal(true), m_albedo(albedo), m_normal(normal) {}

    void SetAlbedo(Texture* albedo) { m_albedo = albedo; }
    void SetNormal(Texture* normal) { m_hasNormal = true; m_normal = normal; }

    void Bind(Shader* shader, uint slot = 0) {
        shader->Set("hasBump", m_hasNormal);
        m_albedo->Bind(slot);
        m_normal->Bind(slot + 1);
    }

    void Unbind(uint slot = 0) {
        m_albedo->Unbind(slot);
        m_normal->Unbind(slot + 1);
    }
};