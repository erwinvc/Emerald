#pragma once

class Tile;

struct TileBufferData {
	float m_x;
	float m_y;
	float m_transformIndex;
	//Vector4 m_heights;

	TileBufferData(float x, float y, int transformIndex/*, const Vector4& heights*/) : m_x(x), m_y(y), m_transformIndex((float)transformIndex)/*, m_heights(heights)*/ {}
	TileBufferData() : m_x(0), m_y(0), m_transformIndex(0)/*, m_heights(Vector4())*/ {}
};

class TileRenderer : public Singleton<TileRenderer> {
public:
	static const int TILECOUNT = 16;

private:
	AssetRef<Texture> texIri;
	AssetRef<Texture> texNoise;
	AssetRef<Shader> m_shader;
	InstancedRenderer2D<TileBufferData>* m_renderers[TILECOUNT];

	TileRenderer() {}
	~TileRenderer() {
		loop(i, TILECOUNT) {
			DELETE(m_renderers[i]);
		}
	}
	friend Singleton;

public:
	void Initialize();
	void Begin();
	void Submit(Tile& tile, int x, int y);
	void End();
	void Draw();

	float m_scale1 = 0;
	float m_scale2 = 0;
	float m_scale3 = 0;
	AssetRef<Material> m_material;
};

static TileRenderer* GetTileRenderer() { return TileRenderer::GetInstance(); }