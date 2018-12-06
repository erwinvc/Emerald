#include "stdafx.h"

int main() {
	srand((UINT)time(NULL));
	Logger::Initialize();
	Application* app = new Application();
	app->Run();
	app->Cleanup();
	delete app;
	Logger::Cleanup();
}