#include "eepch.h"
#include "application/application.h"
#include "graphics/misc/glfw.h"
#include "graphics/misc/DPI.h"
#include "utils/core/log.h"
#include "utils/debug/debug.h"
#include "utils/system/time.h"
#include "graphics/window/window.h"

namespace emerald {
	extern Application* createApplication(const Ref<Window>& mainWindow);

	Ref<Window> initializeEngine() {
		DPI::initialize();
		Time::initialize();
		Debug::initialize();
		Log::initialize("Emerald");

		GLFW::GLFWConfiguration config;
		GLFW::initialize(config);

		auto mainWindow = Ref<Window>::create("Emerald Engine", 10, 10);
		mainWindow->makeContextCurrent();

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			Log::fatal("[GLAD] failed to initialize");
			return nullptr;
		}

		return mainWindow;
	}

	void shutdownEngine() {
		Log::shutdown();
		Debug::shutdown();
		GLFW::shutdown();
	}
}

int main(int argc, char** argv) {
	using namespace emerald;

	Ref<Window> mainWindow = initializeEngine();

	emerald::Application* app = emerald::createApplication(mainWindow);
	app->run();

	delete app;
	emerald::Log::shutdown();

	emerald::shutdownEngine();
	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	using namespace emerald;

	Ref<Window> mainWindow = initializeEngine();

	emerald::Application* app = emerald::createApplication(mainWindow);
	app->run();

	delete app;
	emerald::Log::shutdown();

	emerald::shutdownEngine();
	return 0;
}
