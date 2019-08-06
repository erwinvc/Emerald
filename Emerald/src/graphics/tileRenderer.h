#pragma once

class Tile;

class TileRenderer {
private:
	AssetRef<Texture> texIri;
	AssetRef<Texture> texNoise;
	AssetRef<Shader> m_shader;
    InstancedRenderer2D* m_renderers[5];

    void Initialize();

public:
    TileRenderer() { Initialize(); }
    void Begin();
	void Submit(Tile& tile, int x, int y);
    void Submit(Tile& tile, Vector2& position);
	void End();
	void Draw();

	float m_scale1 = 0;
	float m_scale2 = 0;
	float m_scale3 = 0;
	AssetRef<Material> m_material;
};