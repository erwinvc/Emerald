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

int ImGuiManager::IPFilter(ImGuiTextEditCallbackData* data) {
	ImWchar c = data->EventChar;
	if (c >= '0' && c < '9') return 0;
	if (c == '.' || c == ':') return 0;
	return 1;
}

int ImGuiManager::PathFilter(ImGuiTextEditCallbackData* data) {
	ImWchar c = data->EventChar;
	static char chars[9] = { '<', '>', ':', '"', '/', '\\', '|', '?', '*' };
	for (int i = 0; i < 9; i++) {
		if (c == chars[i]) return 1;
	}
	return 0;
}

void UI::Prepare(const String_t name) {
	ImGui::AlignTextToFramePadding();

	ImGui::Text(name);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
}
void UI::Finish() {
	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

bool UI::BeginWindow(const String_t name, ImVec2 size, bool* open) {
	return ImGui::Begin(name, open, size, -1, GetMouse()->IsLocked() ? ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoDocking: ImGuiWindowFlags_NoDocking);
}

void UI::EndWindow() {
	ImGui::End();
}

void UI::Begin() {
	if (ImGui::GetColumnsCount() != 2) ImGui::Columns(2);
}

void UI::End() {
	if (ImGui::GetColumnsCount() != 1) ImGui::Columns(1);
}

void UI::Header(const String_t name) {
	Prepare(name);
	Finish();
}

void UI::Text(const String_t name, const String& text) {
	Prepare(name);
	ImGui::LabelText(Format_b(buffer, 0x100, "##%s", name), text.c_str());
	Finish();
}

void UI::Bool(const String_t name, bool* value) {
	Prepare(name);
	ImGui::Checkbox(Format_b(buffer, 0x100, "##%s", name), value);
	Finish();
}

void UI::Separator() {
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(0, 4));
	ImGui::Separator();
}

void UI::Dummy(int height) {
	ImGui::Dummy(ImVec2(0, (float)height));
}

void UI::Int(const String_t name, int* value, int min, int max) {
	Prepare(name);
	ImGui::SliderInt(Format_b(buffer, 0x100, "##%s", name), value, min, max);
	Finish();
}

void UI::Float(const String_t name, float* value, float min, float max) {
	Prepare(name);
	ImGui::SliderFloat(Format_b(buffer, 0x100, "##%s", name), value, min, max);
	Finish();
}

void UI::Vec2(const String_t name, glm::vec2* value, float min, float max) {
	Prepare(name);
	ImGui::SliderFloat2(Format_b(buffer, 0x100, "##%s", name), (float*)value, min, max);
	Finish();
}

void UI::Vec3(const String_t name, glm::vec3* value, float min, float max) {
	Prepare(name);
	ImGui::SliderFloat3(Format_b(buffer, 0x100, "##%s", name), (float*)value, min, max);
	Finish();
}

void UI::Vec4(const String_t name, glm::vec4* value, float min, float max) {
	Prepare(name);
	ImGui::SliderFloat4(Format_b(buffer, 0x100, "##%s", name), (float*)value, min, max);
	Finish();
}

void UI::Color3(const String_t name, Color* value) {
	Prepare(name);
	ImGui::ColorEdit3(Format_b(buffer, 0x100, "##%s", name), (float*)value, ImGuiColorEditFlags_NoInputs);
	Finish();
}

void UI::Color4(const String_t name, Color* value) {
	Prepare(name);
	ImGui::ColorEdit4(Format_b(buffer, 0x100, "##%s", name), (float*)value, ImGuiColorEditFlags_NoInputs);
	Finish();
}

bool UI::Combo(const String_t name, int* item, String_t const items[], int itemCount) {
	Prepare(name);
	bool toRet = ImGui::Combo(Format_b(buffer, 0x100, "##%s", name), item, items, itemCount);
	Finish();
	return toRet;
}

namespace ImGui {
	void Tooltip(String_t tooltip) {
		if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.5f) {
			ImGui::SetTooltip(tooltip);
		}
	}

	void BeginDisable(bool disabled) {
		if (!disabled) return;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	void EndDisable(bool disabled) {
		if (!disabled) return;
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}