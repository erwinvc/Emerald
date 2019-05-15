#include "stdafx.h"

ImGuiManager* ImGuiManager::g_instance = nullptr;

void ImGuiManager::Initialize(Window* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.IniFilename = NULL;
	//io.ConfigDockingWithShift = true;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;

	ImGui_ImplGlfw_InitForOpenGL(window->GetHandle(), false);
	ImGui_ImplOpenGL3_Init("#version 410");

	GetGLCallbackManager()->AddOnMouseButtonCallback([](int button, int action, int mods) {
		ImGuiIO& io = ImGui::GetIO();
		if (action == GLFW_PRESS) io.MouseDown[button] = true;
		else if (action == GLFW_RELEASE) io.MouseDown[button] = false;
	});

	GetGLCallbackManager()->AddOnMousePosCallback([](int x, int y) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(x, y);
	});

	GetGLCallbackManager()->AddOnScrollCallback([](double x, double y) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += (float)y;
		io.MouseWheelH += (float)x;
	});

	GetGLCallbackManager()->AddOnKeyCallback([](int key, int scancode, int action, int mods) {
		ImGuiIO& io = ImGui::GetIO();
		if (action == GLFW_PRESS) io.KeysDown[key] = true;
		else if (action == GLFW_RELEASE) io.KeysDown[key] = false;
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	});

	GetGLCallbackManager()->AddOnResizeCallback([](int width, int height) {
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(width, height);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	});

	GetGLCallbackManager()->AddOnKeyTypedEvent([](uint c) {
		ImGuiIO& io = ImGui::GetIO();
		if (c > 0 && c < 0x10000)
			io.AddInputCharacter((unsigned short)c);
	});

}

void ImGuiManager::Begin() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
void ImGuiManager::End() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	GLFWwindow* backup = glfwGetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	glfwMakeContextCurrent(backup);
}
