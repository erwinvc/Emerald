#include "stdafx.h"

void TileRenderer::Initialize() {
	m_shader = GetShaderManager()->Get("Tile");

	AssetRef<Model> full = GetAssetManager()->Get<Model>("Plane");
	AssetRef<Model> inner = GetAssetManager()->Get<Model>("InnerCorner");
	AssetRef<Model> outer = GetAssetManager()->Get<Model>("OuterCorner");
	AssetRef<Model> slope = GetAssetManager()->Get<Model>("Slope");
	AssetRef<Model> valley = GetAssetManager()->Get<Model>("Valley");

	AssetRef<Texture> t = GetAssetManager()->Get<Texture>("White");
	AssetRef<Texture> n = GetAssetManager()->Get<Texture>("BricksNormal");

	m_material = GetMaterialManager()->Create("Tile");
	m_material->SetAlbedo(t);
	m_material->SetNormal(n);

	full->GetMeshes()[full->GetMeshes().size() - 1]->SetMaterial(m_material);
	inner->GetMeshes()[inner->GetMeshes().size() - 1]->SetMaterial(m_material);
	outer->GetMeshes()[outer->GetMeshes().size() - 1]->SetMaterial(m_material);
	slope->GetMeshes()[slope->GetMeshes().size() - 1]->SetMaterial(m_material);
	valley->GetMeshes()[valley->GetMeshes().size() - 1]->SetMaterial(m_material);

	m_renderers[0] = NEW(InstancedRenderer2D(full->GetMeshes()[full->GetMeshes().size() - 1]));
	m_renderers[1] = NEW(InstancedRenderer2D(inner->GetMeshes()[inner->GetMeshes().size() - 1]));
	m_renderers[2] = NEW(InstancedRenderer2D(outer->GetMeshes()[outer->GetMeshes().size() - 1]));
	m_renderers[3] = NEW(InstancedRenderer2D(slope->GetMeshes()[slope->GetMeshes().size() - 1]));
	m_renderers[4] = NEW(InstancedRenderer2D(valley->GetMeshes()[valley->GetMeshes().size() - 1]));

	texIri = GetAssetManager()->Get<Texture>("Irridescence");
	texNoise = GetAssetManager()->Get<Texture>("Noise");
}

void TileRenderer::Begin() {
	m_shader->Bind();
	m_shader->Set("_Boundaries", GetWorld()->GetBoundaries().GetCornerPositions());
	m_shader->Set("viewMatrix", GetCamera()->GetViewMatrix());
	m_shader->Set("projectionMatrix", GetCamera()->GetProjectionMatrix());
	m_shader->Set("texture_iridescence", 4);
	texIri->Bind(4);
	m_shader->Set("texture_noise", 5);
	texNoise->Bind(5);
	m_shader->Set("scale1", m_scale1);
	m_shader->Set("scale2", m_scale2);
	m_shader->Set("scale3", m_scale3);


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
void TileRenderer::End() {
	for (int i = 0; i < 5; i++) {
		m_renderers[i]->End();
	}
}
void TileRenderer::Draw() {
	for (int i = 0; i < 5; i++) {
		m_renderers[i]->Draw(m_shader);
	}
}
