#include "eepch.h"
#include "application/application.h"
#include "graphics/misc/DPI.h"
#include "ui/imguiManager.h"
#include "utils/core/log.h"
#include "utils/debug/debug.h"
#include "utils/system/time.h"
#include <shellapi.h>
#include "utils/threading/threadManager.h"

extern emerald::Application* emerald::createApplication();


int main(int argc, char** argv) {
	//emerald::ThreadManager::registerCurrentThread(emerald::ThreadType::RENDER);
	emerald::DPI::initialize();
	emerald::Time::initialize();
	emerald::Debug::initialize();
	emerald::Log::initialize("Emerald");
	emerald::Application* app = emerald::createApplication();
	emerald::App = app;
	app->run();
	emerald::Log::shutdown();
	delete app;
	emerald::Debug::shutdown();
	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	//emerald::ThreadManager::registerCurrentThread(emerald::ThreadType::RENDER);
	emerald::DPI::initialize();
	emerald::Time::initialize();
	emerald::Debug::initialize();
	emerald::Log::initialize("Emerald");
	emerald::Application* app = emerald::createApplication();
	emerald::App = app;
	app->run();
	emerald::Log::shutdown();
	delete app;
	emerald::Debug::shutdown();
	return 0;
}
