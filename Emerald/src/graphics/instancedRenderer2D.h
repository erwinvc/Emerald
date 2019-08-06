#pragma once

class InstancedRenderer2D {
private:
    bool m_started;
    bool m_ended;
	AssetRef<Mesh> m_mesh = nullptr;
    const int MAX_OBJECTS = 65536* 64;
    ManagedRef<VertexBuffer> m_offsetsBuffer = nullptr;
    int m_amount = 0;
    Vector2* m_offsets = nullptr;
    Vector2* m_offsetsPtr = nullptr;

    void Initialize();
public:
    void Begin();
	void Submit(int x, int y);
    void Submit(Vector2& offset);
    void End();
    void Draw(AssetRef<Shader> shader);

    InstancedRenderer2D(AssetRef<Mesh> mesh) : m_started(false), m_ended(true), m_mesh(mesh) { Initialize(); }
    ~InstancedRenderer2D();
};