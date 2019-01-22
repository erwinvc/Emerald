#include "stdafx.h"

ImGuiManager* ImGuiManager::g_instance = nullptr;

void ImGuiManager::Initialize(Window* window) {
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), false);
    ImGui_ImplOpenGL3_Init("#version 410");
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
