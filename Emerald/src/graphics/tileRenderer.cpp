#include "stdafx.h"

void TileRenderer::Initialize() {
	m_shader = GetShaderManager()->Get("Tile");

	AssetRef<Texture> t = GetAssetManager()->Get<Texture>("White");
	AssetRef<Texture> n = GetAssetManager()->Get<Texture>("BricksNormal");

	m_material = GetMaterialManager()->Create<TileMaterial>("Tile");

	BufferLayout layout = {
	{VertexBufferDataType::Float2, "position", 5, true},
	{VertexBufferDataType::Int, "transformIndex", 6, true},
	{VertexBufferDataType::Float3, "vsStrengths", 7, true},
	{VertexBufferDataType::Int, "vsTextureID", 8, true}
	};

	loop(i, TILECOUNT) {
		AssetRef<Model> model = GetAssetManager()->Get<Model>(Format("Tile[%d]", i));
		AssetRef<Mesh> mesh = model->GetMeshes()[model->GetMeshes().size() - 1];
		mesh->SetMaterial(m_material);
		m_renderers[i] = NEW(InstancedRenderer2D<TileBufferData>(mesh, 65536, layout));
	}

	texIri = GetAssetManager()->Get<Texture>("Irridescence");
	texNoise = GetAssetManager()->Get<Texture>("Noise");

	Matrix4 transforms[4] = {
		Matrix4::Rotate(0, Vector3::YAxis()),
		Matrix4::Rotate(Math::HALF_PI + Math::PI, Vector3::YAxis()),
		Matrix4::Rotate(Math::PI, Vector3::YAxis()),
		Matrix4::Rotate(Math::HALF_PI, Vector3::YAxis()),
	};

	m_shader->Bind();
	//for (int i = 0; i < NUMOF(transforms); i++) {
	//	m_shader->Set(Format_t("_Transforms[%d]", i), transforms[i]);
	//}
	m_shader->Set("_Transforms", transforms);

}

void TileRenderer::Begin() {
	m_shader->Bind();
	m_shader->Set("_Boundaries", GetWorld()->GetBoundaries().GetCornerPositions());
	m_shader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
	m_shader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
	m_shader->Set("_Iridescence", 4);
	texIri->Bind(4);

	m_shader->Set("scale1", m_scale1);
	m_shader->Set("scale2", m_scale2);
	m_shader->Set("_IridescenceStrength", m_scale3);

	loop(i, TILECOUNT) {
		m_renderers[i]->Begin();
	}
}
void TileRenderer::Submit(Tile& tile, int x, int y) {
	m_renderers[tile.m_type]->Submit(TileBufferData((float)x, (float)y, tile.m_transformIndex, m_scale2, 1, 1, tile.m_definition->GetMaterialID()));
}
//void TileRenderer::Submit(Tile& tile, Vector2& position) {
//	m_renderers[tile.m_type]->Submit(position);
//}
void TileRenderer::End() {
	loop(i, TILECOUNT) {
		m_renderers[i]->End();
	}
}
void TileRenderer::Draw() {
	loop(i, TILECOUNT) {
		m_renderers[i]->Draw(m_shader);
	}
}
