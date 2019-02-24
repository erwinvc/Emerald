#include "stdafx.h"

ImGuiManager* ImGuiManager::g_instance = nullptr;

void ImGuiManager::Initialize(Window* window) {
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

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
        io.MouseWheel += y;
        io.MouseWheelH += x;
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
    //ImGuiIO& io = ImGui::GetIO();
    //io.DisplaySize = ImVec2(GetApplication()->GetWindow()->GetWidth(), GetApplication()->GetWindow()->GetHeight());

    //float time = (float)glfwGetTime();
    //io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
    //m_time = time;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void ImGuiManager::End() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
