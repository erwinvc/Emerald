#include "stdafx.h"

Application* app;

int main() {
    srand((UINT)time(NULL));
    Logger::Initialize();
    app = new Application();
    app->Run();
    app->Cleanup();
    delete app;
    Logger::Cleanup();
}