#include "stdafx.h"

ImFont* ImGuiManager::g_bigFont;
ImFont* ImGuiManager::g_smallFont;
char UI::buffer[0x100];

void ImGuiManager::Initialize(Window* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	io.IniFilename = NULL;
	//io.ConfigDockingWithShift = true;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//ImGui::StyleColorsDark();
	ApplyStyle();
	
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;

	ImGui_ImplGlfw_InitForOpenGL(window->GetHandle(), false);
	ImGui_ImplOpenGL3_Init("#version 330");

	GetGLCallbackManager()->AddOnMouseButtonCallback([](int button, int action, int mods) {
		ImGuiIO& io = ImGui::GetIO();
		if (action == GLFW_PRESS) io.MouseDown[button] = true;
		else if (action == GLFW_RELEASE) io.MouseDown[button] = false;
	});

	GetGLCallbackManager()->AddOnMousePosCallback([](int x, int y) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2((float)x, (float)y);
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
		io.DisplaySize = ImVec2((float)width, (float)height);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	});

	GetGLCallbackManager()->AddOnKeyTypedEvent([](uint c) {
		ImGuiIO& io = ImGui::GetIO();
		if (c > 0 && c < 0x10000)
			io.AddInputCharacter((unsigned short)c);
	});

	ImGui::GetIO().Fonts->AddFontDefault();
	g_smallFont = io.FontDefault;
	g_bigFont = io.FontDefault;
	
	io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/Consolas.ttf", 15.0f, NULL);
	io.Fonts->Build();
	m_initialized = true;
}

void ImGuiManager::ApplyStyle() {
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.05f, 0.04f, 0.86f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.31f, 0.24f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.53f, 0.40f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.11f, 0.76f, 0.53f, 0.54f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.33f, 0.21f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.32f, 0.42f, 0.36f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.43f, 0.56f, 0.47f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.60f, 0.78f, 0.66f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.98f, 0.35f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.18f, 0.56f, 0.30f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.18f, 0.56f, 0.30f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.13f, 0.42f, 0.23f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.56f, 0.30f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.31f, 0.78f, 0.47f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.13f, 0.42f, 0.23f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.56f, 0.30f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.31f, 0.78f, 0.47f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.13f, 0.42f, 0.23f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.18f, 0.56f, 0.30f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.31f, 0.78f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.13f, 0.42f, 0.23f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.18f, 0.56f, 0.30f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.31f, 0.78f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.05f, 0.24f, 0.11f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.66f, 0.31f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.13f, 0.42f, 0.23f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.30f, 0.18f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.18f, 0.56f, 0.30f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.13f, 0.42f, 0.23f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.13f, 0.42f, 0.23f, 1.00f);
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

bool UI::BeginWindow(const String_t name) {
	bool open = true;
	return ImGui::Begin(Format_t("%s###Window", name), &open, ImVec2(576, 680), -1, GetMouse()->IsLocked() ? ImGuiWindowFlags_NoMouseInputs : 0);
}