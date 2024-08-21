#include "eepch.h"
#include "editorWindow.h"
#include "imgui.h"
#include <GLFW/glfw3.h>
#include "graphics/window.h"
#include "ui/iconsFontAwesome.h"
#include "ui/imguiManager.h"
#include "ui/iconsFontSegoeMDL2.h"
#include "editor.h"
#include "graphics/renderPipeline.h"
#include "graphics/renderPass.h"
#include "graphics/frameBuffer.h"
#include "graphics/texture.h"
#include "graphics/engineIcon.h"
#include "graphics/renderer.h"
#include "imguiProfiler/Profiler.h"
#include "project.h"

#include <imgui_internal.h>
#include "util/fileSystem.h"
#include "metrics/metrics.h"
#include "debugWindow.h"

namespace emerald {
	static Ref<Texture> s_icon;
	static bool s_mouseInViewport = false;
	static bool s_viewportFocused = false;
	static bool s_TitleBarHovered = false;

	EditorWindow::~EditorWindow() {
		s_icon.reset();
	}

	void EditorWindow::initialize() {
		glfwSetTitlebarHitTestCallback(App->getWindow()->handle(), [](GLFWwindow* window, int x, int y, int* hit) {
			*hit = s_TitleBarHovered;
		});

		TextureDesc desc;
		desc.m_hasMipmaps = false;
		desc.m_anisotropyLevel = 0;
		desc.m_format = RGBA;
		desc.m_readWrite = false;
		desc.m_filter = NEAREST;

		s_icon = Ref<Texture>::create(desc, 32, 32, icon::icon32_map, NUMOF(icon::icon32_map), TextureDataType::FILE);
		Renderer::submit([] { s_icon->invalidate(); });
	}

	void drawMenuBar() {
		bool open = false;
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New project", "Ctrl+N")) {
					FileSystem::openFolderDialog(L"Choose location for new project");
				}
				if (ImGui::MenuItem("Open project", "Ctrl+O")) {
					FileSystem::openFileDialog({ {L"Emerald Project Files", L"*.eep"} });
				}
				if (ImGui::BeginMenu("Recent projects")) {
					ImGui::BeginDisabled(true);
					ImGui::MenuItem("Empty");
					ImGui::EndDisabled();
					//Add projects here
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
					App->close();
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("View")) {}
			if (ImGui::BeginMenu("Window")) {
				if (ImGui::MenuItem("Profiler")) {
					EditorWindows.profiler = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		//if (open)ImGui::OpenPopup("Select Project");

		Project::showProjectPopup(open);
	}

	void EditorWindow::drawTitlebar(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight) {
		const bool maximized = App->getWindow()->isMaximized();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollbar;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, maximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize(size);
		ImGui::SetNextWindowViewport(viewportID);

		ImGui::Begin("TitleBarWindow", nullptr, window_flags);
		ImGuiManager::pushFont(ImGUIFont::SEGOE);

		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;
		const float titleBarButtonSize = 46;
		const float buttonsAreaWidth = titleBarButtonSize * 4;
		const ImVec2 buttonSize = ImVec2(titleBarButtonSize, 46);

		//Titlebar
		const float contentRegionWidth = ImGui::GetContentRegionAvail().x;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + (maximized ? 2.0f : 0.0f)));
		ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth(), titlebarHeight });

		//Logo
		ImGui::BeginHorizontal("TitlebarLogo", ImVec2(46, 46), 0.5f);
		ImGui::Spring();
		ImGui::Image((void*)(uint64_t)s_icon->handle(), ImVec2(32.0f, 32.0f));
		ImGui::Spring();
		ImGui::EndHorizontal();
		s_TitleBarHovered = ImGui::IsItemHovered();

		//Title and subtitle
		ImVec2 backupPos = ImGui::GetCursorPos();
		ImGuiManager::pushFont(ImGUIFont::INTER);
		ImGui::BeginVertical("TitlebarTitle", ImVec2(0, 46), 0.0f);
		ImGui::Spring();
		ImGui::Text((EditorHeader.title + " - Untitled - D:\\Emerald").c_str());
		ImGui::BeginDisabled(true);
		ImGui::Text(EditorHeader.subTitle.c_str());
		ImGui::EndDisabled();
		ImGui::Spring();
		ImGui::EndVertical();
		ImGuiManager::popFont();
		ImGui::SetCursorPos(backupPos);
		s_TitleBarHovered = ImGui::IsItemHovered();

		ImGui::InvisibleButton("", ImVec2(contentRegionWidth - buttonsAreaWidth + 2, titlebarHeight));
		s_TitleBarHovered |= ImGui::IsItemHovered();

		//Buttons
		ImGui::PushStyleColor(ImGuiCol_Button, Color(0x1F1F1FFF));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Color(0x3D3D3DFF));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, Color(0x383838FF));
		if (ImGui::Button((const char*)SEGOE_MDL2_ICON_CHROME_MINIMIZE, buttonSize)) {
			App->QueueEvent([] {App->getWindow()->minimize(); });
		}

		auto& buttonLabel = maximized ? SEGOE_MDL2_ICON_CHROME_RESTORE : SEGOE_MDL2_ICON_CHROME_MAXIMIZE;
		if (ImGui::Button((const char*)buttonLabel, buttonSize)) {
			App->QueueEvent([maximized] {
				if (maximized) App->getWindow()->restore();
				else App->getWindow()->maximize();
			});
		}
		if (ImGui::Button((const char*)SEGOE_MDL2_ICON_CHROME_CLOSE, buttonSize)) App->close();
		ImGui::PopStyleColor(3);
		ImGui::EndHorizontal();
		ImGuiManager::popFont();

		ImGui::PopStyleVar(2);

		ImGui::End();
	}

	void drawAppArea(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

		const bool maximized = App->getWindow()->isMaximized();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, maximized ? ImVec2(4.0f, 0.0f) : ImVec2(0, 0.0f));
		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize(size);
		ImGui::SetNextWindowViewport(viewportID);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		auto dockspace_id = ImGui::GetID("MainViewportDockspace");

		static auto first_time = true;
		if (first_time) {
			first_time = false;
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
			ImGuiID dockspace_main_id = dockspace_id;

			ImGuiID right = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_main_id);
			ImGuiID left = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Left, 0.25f, nullptr, &dockspace_main_id);
			ImGuiID down = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_main_id);

			ImGui::DockBuilderDockWindow("Inspector", right);
			ImGui::DockBuilderDockWindow("Debug", right);
			ImGui::DockBuilderDockWindow("Dear ImGui Demo", right);
			ImGui::DockBuilderDockWindow("Hierarchy", left);
			ImGui::DockBuilderDockWindow("Assets", down);
			ImGui::DockBuilderDockWindow("Log", down);
			ImGui::DockBuilderDockWindow("Viewport", dockspace_main_id);
			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGui::DockSpace(dockspace_id);
		drawMenuBar();
		ImGui::End();
	}

	//Windows

	void drawViewport() {
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNav;

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::Begin("Viewport", nullptr, windowFlags);
		s_mouseInViewport = ImGui::IsWindowHovered();
		s_viewportFocused = ImGui::IsWindowFocused();
		if (s_mouseInViewport) {
			ImGui::SetNextFrameWantCaptureMouse(false);
		}
		if (s_viewportFocused) {
			ImGui::SetNextFrameWantCaptureKeyboard(false);
		}

		ImGui::Image((void*)(uint64_t)Editor->getFinalTexture()->handle(), ImGui::GetContentRegionAvail());

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void drawWindows() {
		drawViewport();
		DebugWindow::draw();

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoNav);
		ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::TOP);
		ImGui::End();

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoNav);
		ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::TOP);
		ImGui::End();

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoNav);
		ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::TOP);
		ImGui::End();

		if (EditorWindows.profiler) {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(800, 300));
			ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
			if (ImGui::Begin("Profiler", &EditorWindows.profiler)) {
				ImGuiManager::pushFont(ImGUIFont::INTER);
				DrawProfilerHUD();
				ImGuiManager::popFont();
			}
			ImGui::PopStyleVar();
			ImGui::End();

			if (!EditorWindows.profiler) {
				PROFILE_DISABLE();
			}
		}

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoNav);
		ImGui::DrawGradientBackgroundForWindow(ImGui::GradientDirection::TOP);
		ImGui::Button("TestButton");
		if (ImGui::CollapsingHeader("Header2", ImGuiTreeNodeFlags_SpanFullWidth)) {
			ImGui::Text("TestText");
			ImGui::Button("TestButton");
		}
		if (ImGui::CollapsingHeader("Header3", ImGuiTreeNodeFlags_SpanAvailWidth)) {
			ImGui::Text("TestText");
			ImGui::Button("TestButton");
		}
		if (ImGui::CollapsingHeader("Header4", ImGuiTreeNodeFlags_CollapsingHeader)) {
			ImGui::Text("TestText");
			ImGui::Button("TestButton");
		}
		static bool b;
		ImGui::Checkbox("TestCheckbox", &b);
		static float v;
		ImGui::SliderFloat("TestSlider", &v, 0.0f, 1.0f);
		ImGui::End();
	}

	void EditorWindow::update(Timestep ts) {

	}

	void EditorWindow::onImGuiRender() {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		float titlebarYOffset = App->getWindow()->isMaximized() ? 8.0f : 0.0f;
		const float titleBarHeight = 44;

		drawTitlebar(viewport->Pos, ImVec2(viewport->Size.x, titleBarHeight + titlebarYOffset), viewport->ID, titleBarHeight);
		drawAppArea(ImVec2(viewport->Pos.x, viewport->Pos.y + titleBarHeight + titlebarYOffset), ImVec2(viewport->Size.x, viewport->Size.y - titleBarHeight - titlebarYOffset), viewport->ID, titleBarHeight);

		ImGui::PopStyleVar(2);

		drawWindows();

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::ShowDemoWindow(nullptr);
	}

	void EditorWindow::fixedUpdate(Timestep ts) {

	}

	void EditorWindow::destroy() {

	}
}