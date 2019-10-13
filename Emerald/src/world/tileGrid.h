#pragma once

class TileGrid {
private:
	struct TileBufferData {
		float m_x;
		float m_y;
		Vector2 m_rotation;
		float m_textureID;

		TileBufferData(float x, float y, float rx, float ry, int tid) : m_x(x), m_y(y), m_rotation(Vector2(rx, ry)), m_textureID(tid) {}
		TileBufferData() : m_x(0), m_y(0), m_rotation(Vector2()), m_textureID(0) {}
	};

	InstancedRenderer<TileBufferData>* m_renderer;
	Tile m_tiles[32][32];
	AssetRef<Shader> m_tileShader;
	const float TILESIZE = 10;
public:
	TileGrid() {
		BufferLayout layout = {
			{VertexBufferDataType::Float2, "vsPosition", 5, true},
			{VertexBufferDataType::Float2, "vsRotation", 6, true},
			{VertexBufferDataType::Int, "vsTextureID", 7, true}
		};

		m_renderer = new InstancedRenderer<TileBufferData>(GetAssetManager()->Get<Model>("Tile")->GetMeshes()[0], 8192*8, layout);

		m_tileShader = GetShaderManager()->Get("Tile");
		m_tileShader->Bind();
		m_tileShader->Set("_Albedo", 0);
		m_tileShader->Set("_Normal", 1);
		m_tileShader->Set("_Roughness", 2);
		m_tileShader->Set("_Metallic", 3);
		m_tileShader->Set("_Emission", 4);

		loop(x, 32) {
			loop(y, 32) {
				m_tiles[y][x] = Tile(Vector2I(x, y), 0);
			}
		}
	}

	~TileGrid() {
		delete m_renderer;
	}

	void DrawGeometry();
};