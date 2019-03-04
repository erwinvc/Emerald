#include "stdafx.h"

void TileRenderer::Initialize() {
    m_shader = new Shader("Tile", "src/shader/tile.vert", "src/shader/tile.frag");
    Model* full = new Model();
    full->LoadModel("sponza/Full.obj");

    Model* inner = new Model();
    inner->LoadModel("sponza/InnerCorner.obj");

    Model* outer = new Model();
    outer->LoadModel("sponza/OuterCorner.obj");

    Model* slope = new Model();
    slope->LoadModel("sponza/Slope.obj");

    Model* valley = new Model();
    valley->LoadModel("sponza/Valley.obj");

    Texture* t = new Texture("res/white.png");
    Texture* n = new Texture("res/testNormal.png");
    Material* m = new Material(t, n);

    full->GetMeshes()[full->GetMeshes().size() - 1]->SetMaterial(m);
    inner->GetMeshes()[inner->GetMeshes().size() - 1]->SetMaterial(m);
    outer->GetMeshes()[outer->GetMeshes().size() - 1]->SetMaterial(m);
    slope->GetMeshes()[slope->GetMeshes().size() - 1]->SetMaterial(m);
    valley->GetMeshes()[valley->GetMeshes().size() - 1]->SetMaterial(m);

    m_renderers[0] = new InstancedRenderer2D(full->GetMeshes()[full->GetMeshes().size() - 1]);
    m_renderers[1] = new InstancedRenderer2D(inner->GetMeshes()[inner->GetMeshes().size() - 1]);
    m_renderers[2] = new InstancedRenderer2D(outer->GetMeshes()[outer->GetMeshes().size() - 1]);
    m_renderers[3] = new InstancedRenderer2D(slope->GetMeshes()[slope->GetMeshes().size() - 1]);
    m_renderers[4] = new InstancedRenderer2D(valley->GetMeshes()[valley->GetMeshes().size() - 1]);
}

void TileRenderer::Begin(Camera* cam, Matrix4& proj/*#TODO Dirty*/) {
    m_shader->Reload();
    m_shader->Bind();
    m_shader->Set("viewMatrix", cam->GetViewMatrix());
    m_shader->Set("projectionMatrix", proj);
    m_shader->Set("uDiffTex", 0);
    m_shader->Set("uBumpTex", 1);
    for (int i = 0; i < 5; i++) {
        m_renderers[i]->Begin();
    }

}
void TileRenderer::Submit(Tile& tile, Vector2& position) {
    m_renderers[tile.m_type]->Submit(position);
}
void TileRenderer::Draw() {
    for (int i = 0; i < 5; i++) {
        m_renderers[i]->End();
        m_renderers[i]->Draw(m_shader);
    }
}
