#pragma once

//class Tile;
//
//struct TileBufferData {
//	float m_x;
//	float m_y;
//	int m_transformIndex;
//	float m_strengthsNormal;
//	float m_strengthsSpecular;
//	float m_strengthsEmission;
//	int m_textureID;
//	//Vector4 m_heights;
//
//	TileBufferData(float x, float y, int transformIndex, float normalStrength, float specularStrength, float emissionStrength, int textureID) :
//		m_x(x), m_y(y),
//		m_transformIndex(transformIndex),
//		m_strengthsNormal(normalStrength),
//		m_strengthsSpecular(specularStrength),
//		m_strengthsEmission(emissionStrength),
//		m_textureID(textureID) {
//	}
//
//	TileBufferData() : m_x(0), m_y(0), m_transformIndex(0),
//		m_strengthsNormal(0),
//		m_strengthsSpecular(0),
//		m_strengthsEmission(0),
//		m_textureID(0) {
//	}
//};
//
//class TileRenderer : public Singleton<TileRenderer> {
//public:
//	static const int TILECOUNT = 16;
//
//private:
//	AssetRef<Texture> texIri;
//	AssetRef<Texture> texNoise;
//	AssetRef<Shader> m_shader;
//	InstancedRenderer<TileBufferData>* m_renderers[TILECOUNT];
//
//	TileRenderer() {}
//	~TileRenderer() {
//		loop(i, TILECOUNT) {
//			DELETE(m_renderers[i]);
//		}
//	}
//	friend Singleton;
//
//public:
//	void Initialize();
//	void Begin();
//	void Submit(Tile& tile, int x, int y);
//	void End();
//	void Draw();
//
//	float m_scale1 = 0;
//	float m_scale2 = 2;
//	float m_scale3 = 0;
//	float roughness = 0;
//	float metallic = 0;
//	AssetRef<TileMaterial> m_material;
//};
//
//static TileRenderer* GetTileRenderer() { return TileRenderer::GetInstance(); }