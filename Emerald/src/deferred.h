#pragma once

class Deferred {
public:
    Window* m_window;
    void Initialize(Window* window, FreeCam& camera);
    void Render();
    void Update();
    void Resize(uint width, uint height);
};