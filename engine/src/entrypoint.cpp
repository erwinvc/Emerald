#include "eepch.h"
#include "stdIncl.h"
#include "application.h"
#include <ctime>

extern emerald::Application* emerald::createApplication(int argc, char** argv);
bool g_applicationRunning = true;

int main(int argc, char** argv) {
	srand((uint32_t)time(NULL));
	emerald::Log::initialize("Emerald");
	emerald::Application* app = emerald::createApplication(argc, argv);
	emerald::App = app;
	app->run();
	delete app;
	emerald::Log::cleanup();
}