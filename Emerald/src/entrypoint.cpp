#include "stdafx.h"

Application* app;

int main() {
    srand((UINT)time(NULL));
    Logger::Initialize();
    app = GetApplication();
    app->Run();
    app->Cleanup();
    Logger::Cleanup();
}