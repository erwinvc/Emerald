#pragma once

class InstancedRenderer2D {
private:
    bool m_started;
    bool m_ended;
    Mesh* m_mesh = nullptr;
    const uint MAX_OBJECTS = 65536;
    Buffer* m_offsetsBuffer = nullptr;
    int m_amount = 0;
    Vector2* m_offsets = nullptr;
    Vector2* m_offsetsPtr = nullptr;

    void Initialize();
public:
    void Begin();
	void Submit(int x, int y);
    void Submit(Vector2& offset);
    void End();
    void Draw(Shader* shader);

    InstancedRenderer2D(Mesh* mesh) : m_started(false), m_ended(true), m_mesh(mesh) { Initialize(); }
    ~InstancedRenderer2D();
};