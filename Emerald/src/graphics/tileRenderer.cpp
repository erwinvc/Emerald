#include "stdafx.h"

void TileRenderer::Initialize() {
	m_shader = NEW(Shader("Tile", "src/shader/tile.vert", "src/shader/tile.frag"));
	Model* full = NEW(Model());
	full->LoadModel("tiles/Plane.fbx");

	Model* inner = NEW(Model());
	inner->LoadModel("tiles/Inner Corner.fbx");

	Model* outer = NEW(Model());
	outer->LoadModel("tiles/Outer Corner.fbx");

	Model* slope = NEW(Model());
	slope->LoadModel("tiles/Slope.fbx");

	Model* valley = NEW(Model());
	valley->LoadModel("tiles/Valley.fbx");

	Texture* t = NEW(Texture("res/white.png"));
	Texture* n = NEW(Texture("sponza/bricksNormal.png"));
	Material* m = NEW(Material());
	m->SetAlbedo(t)->SetNormal(n);

	material = m;
	full->GetMeshes()[full->GetMeshes().size() - 1]->SetMaterial(m);
	inner->GetMeshes()[inner->GetMeshes().size() - 1]->SetMaterial(m);
	outer->GetMeshes()[outer->GetMeshes().size() - 1]->SetMaterial(m);
	slope->GetMeshes()[slope->GetMeshes().size() - 1]->SetMaterial(m);
	valley->GetMeshes()[valley->GetMeshes().size() - 1]->SetMaterial(m);

	m_renderers[0] = NEW(InstancedRenderer2D(full->GetMeshes()[full->GetMeshes().size() - 1]));
	m_renderers[1] = NEW(InstancedRenderer2D(inner->GetMeshes()[inner->GetMeshes().size() - 1]));
	m_renderers[2] = NEW(InstancedRenderer2D(outer->GetMeshes()[outer->GetMeshes().size() - 1]));
	m_renderers[3] = NEW(InstancedRenderer2D(slope->GetMeshes()[slope->GetMeshes().size() - 1]));
	m_renderers[4] = NEW(InstancedRenderer2D(valley->GetMeshes()[valley->GetMeshes().size() - 1]));
}

void TileRenderer::Begin() {
	m_shader->Reload();
	m_shader->Bind();
	m_shader->Set("_Boundaries", GetWorld()->GetBoundaries().GetCornerPositions());
	m_shader->Set("viewMatrix", GetCamera()->GetViewMatrix());
	m_shader->Set("projectionMatrix", GetCamera()->GetProjectionMatrix());

	for (int i = 0; i < 5; i++) {
		m_renderers[i]->Begin();
	}
}
void TileRenderer::Submit(Tile& tile, int x, int y) {
	m_renderers[tile.m_type]->Submit(x, y);
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
