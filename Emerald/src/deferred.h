#pragma once

class Deferred {
public:
    Window* m_window;
    void HandleInput();
    void Initialize(Window* window, FreeCam& camera);
    void Render();
};