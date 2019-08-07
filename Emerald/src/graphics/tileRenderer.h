#pragma once

class Tile;

struct TileBufferData {
	float m_x;
	float m_y;
	float m_rotation;

	TileBufferData(float x, float y, float rotation) : m_x(x), m_y(y), m_rotation(rotation) {}
	TileBufferData() : m_x(0), m_y(0), m_rotation(0) {}
};

class TileRenderer {
private:
	AssetRef<Texture> texIri;
	AssetRef<Texture> texNoise;
	AssetRef<Shader> m_shader;
	InstancedRenderer2D<TileBufferData>* m_renderers[5];

	void Initialize();

public:
	TileRenderer() { Initialize(); }
	void Begin();
	void Submit(Tile& tile, int x, int y);
	void End();
	void Draw();

	float m_scale1 = 0;
	float m_scale2 = 0;
	float m_scale3 = 0;
	AssetRef<Material> m_material;
};