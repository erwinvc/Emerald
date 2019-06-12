#pragma once

class Tile;

class TileRenderer {
private:
	Texture* texIri;
	Texture* texNoise;
    Shader* m_shader;
    InstancedRenderer2D* m_renderers[5];

    void Initialize();
public:
    Material* material;
    TileRenderer() { Initialize(); }
    void Begin();
	void Submit(Tile& tile, int x, int y);
    void Submit(Tile& tile, Vector2& position);
    void Draw();

	float scale1 = 0;
	float scale2 = 0;
	float scale3 = 0;
};