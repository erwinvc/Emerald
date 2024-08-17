#include "eepch.h"
#include "stdIncl.h"
#include "application.h"
#include <ctime>
#include <shellapi.h>

extern emerald::Application* emerald::createApplication();


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	srand((uint32_t)time(NULL));
	emerald::Log::initialize("Emerald");
	emerald::Application* app = emerald::createApplication();
	emerald::App = app;
	app->run();
	delete app;
	emerald::Log::cleanup();
	return 0;
}
