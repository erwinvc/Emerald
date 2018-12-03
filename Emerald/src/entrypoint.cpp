#include "stdafx.h"

int main() {
	Logger::Initialize();
	Application* app = new Application();
	app->Run();
	app->Cleanup();
	delete app;
	Logger::Cleanup();
}