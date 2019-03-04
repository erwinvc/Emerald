#pragma once

class InstancedRenderer{
private:
    bool m_started;
    bool m_ended;
    Mesh* m_mesh;
    const uint MAX_OBJECTS = 4096;
    Buffer* m_offsetsBuffer;
    int m_amount;
    int m_numindices;
    Vector3* m_offsets;
    Vector3* m_offsetsPtr;

    void Initialize();
public:
    void Begin();
    void Submit(Vector3& offset);
    void End();
    void Draw();

    InstancedRenderer(Mesh* mesh) : m_mesh(mesh) { Initialize(); }
    ~InstancedRenderer();
};