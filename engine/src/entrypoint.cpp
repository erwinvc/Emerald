#include "eepch.h"
#include "stdIncl.h"
#include "application.h"
#include <shellapi.h>
#include "util/time.h"
#include "util/debug.h"
#include "graphics/DPI.h"

extern emerald::Application* emerald::createApplication();


int main(int argc, char** argv) {
	emerald::DPI::initialize();
	emerald::Time::initialize();
	emerald::Debug::initialize();
	emerald::Log::initialize("Emerald");
	emerald::Application* app = emerald::createApplication();
	emerald::App = app;
	app->run();
	delete app;
	emerald::Log::shutdown();
	emerald::Debug::shutdown();
	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	emerald::DPI::initialize();
	emerald::Time::initialize();
	emerald::Debug::initialize();
	emerald::Log::initialize("Emerald");
	emerald::Application* app = emerald::createApplication();
	emerald::App = app;
	app->run();
	delete app;
	emerald::Log::shutdown();
	emerald::Debug::shutdown();
	return 0;
}
