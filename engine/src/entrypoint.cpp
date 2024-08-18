#include "eepch.h"
#include "stdIncl.h"
#include "application.h"
#include <ctime>
#include <shellapi.h>

extern emerald::Application* emerald::createApplication();

int main(int argc, char** argv) {
	srand((uint32_t)time(NULL));
	emerald::Log::initialize("Emerald");
	emerald::Application* app = emerald::createApplication();
	emerald::App = app;
	app->run();
	delete app;
	emerald::Log::cleanup();
	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	srand((uint32_t)time(NULL));
	emerald::Log::initialize("Emerald");
	emerald::Application* app = emerald::createApplication();
	emerald::App = app;
	app->run();
	delete app;
	emerald::Log::cleanup();
	return 0;
}
