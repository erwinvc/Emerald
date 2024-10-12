#include "eepch.h"
#include "debugWindow.h"
#include "editor.h"
#include "editorWindow.h"
#include "graphics/engineIcon.h"
#include "graphics/frameBuffer.h"
#include "graphics/renderer.h"
#include "graphics/renderPass.h"
#include "graphics/renderPipeline.h"
#include "graphics/texture.h"
#include "graphics/window.h"
#include "hierarchyTree.h"
#include "imgui.h"
#include "imguiProfiler/Profiler.h"
#include "metrics/metrics.h"
#include "profilerWindow.h"
#include "project.h"
#include "scene/sceneManager.h"
#include "ui/iconsFontAwesome.h"
#include "ui/iconsFontSegoeMDL2.h"
#include "ui/imguiManager.h"
#include "util/fileSystem.h"
#include <GLFW/glfw3.h>
#include <util/valueTester.h>
#include "graphics/DPI.h"

namespace emerald {
	static bool s_mouseInViewport = false;
	static bool s_viewportFocused = false;
	static bool s_TitleBarHovered = false;
	static glm::vec2 s_sceneViewportSize = glm::vec2(1.0f, 1.0f);

	EditorWindow::EditorWindow() {
		glfwSetTitlebarHitTestCallback(App->getWindow()->handle(), [](GLFWwindow* window, int x, int y, int* hit) {
			*hit = s_TitleBarHovered;
		});

		TextureDesc desc;
		desc.hasMipmaps = false;
		desc.anisotropyLevel = 0;
		desc.format = RGBA8F;
		desc.readWrite = false;
		desc.filter = NEAREST;

		m_icon = Ref<Texture>::create(desc, 32, 32, icon::icon32_map, NUMOF(icon::icon32_map), TextureDataType::FILE);
		Renderer::submit([instance = Ref<Texture>(m_icon)]() mutable { instance->invalidate(); });
	}

	EditorWindow::~EditorWindow() {
		m_icon.reset();
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
				if (ImGui::MenuItem("ImGui demo")) {
					EditorWindows.demo = true;
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
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoScrollbar;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, maximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize(size);
		ImGui::SetNextWindowViewport(viewportID);

		ImGui::Begin("TitleBarWindow", nullptr, window_flags);
		ImGuiManager::pushFont(ImGUIFont::SEGOE_TITLEBAR);

		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;
		const float titleBarButtonSize = DPI::getScale(46.0f);
		const float buttonsAreaWidth = titleBarButtonSize * 4;
		const ImVec2 buttonSize = ImVec2(titleBarButtonSize, DPI::getScale(46.0f));

		//Titlebar
		const float contentRegionWidth = ImGui::GetContentRegionAvail().x;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + (maximized ? 2.0f : 0.0f)));
		ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth(), titlebarHeight });

		//Logo
		ImGui::BeginHorizontal("TitlebarLogo", ImVec2(titleBarButtonSize, titleBarButtonSize), 0.5f);
		ImGui::Spring();
		ImGui::Image((void*)(uint64_t)m_icon->handle(), DPI::getScale(ImVec2(32.0f, 32.0f)));
		ImGui::Spring();
		ImGui::EndHorizontal();
		s_TitleBarHovered = ImGui::IsItemHovered();

		//Title and subtitle
		ImVec2 backupPos = ImGui::GetCursorPos();
		ImGuiManager::pushFont(ImGUIFont::INTER);
		ImGui::BeginVertical("TitlebarTitle", ImVec2(0, titleBarButtonSize), 0.0f);
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
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
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

		ImGui::PopStyleVar(3);

		ImGui::End();
	}

	void EditorWindow::drawEditor(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

		const bool maximized = App->getWindow()->isMaximized();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, maximized ? ImVec2(4.0f, 4.0f) : ImVec2(0.0f, 0.0f));
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

			ImGuiID right = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Right, 0.3f, nullptr, &dockspace_main_id);
			ImGuiID down = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_main_id);
			ImGuiID left = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Left, 0.25f, nullptr, &dockspace_main_id);
			ImGuiID rightDown = ImGui::DockBuilderSplitNode(right, ImGuiDir_Down, 0.25f, nullptr, &right);

			ImGui::DockBuilderDockWindow("Debug", right);
			ImGui::DockBuilderDockWindow("Inspector", right);
			ImGui::DockBuilderDockWindow("Log", down);
			ImGui::DockBuilderDockWindow("Assets", down);
			ImGui::DockBuilderDockWindow("Hierarchy", left);
			ImGui::DockBuilderDockWindow("Scene", rightDown);
			ImGui::DockBuilderDockWindow("Viewport", dockspace_main_id);
			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGui::DockSpace(dockspace_id);
		drawMenuBar();
		ImGui::End();
	}

	//Windows
	void EditorWindow::drawViewport() {
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNav;

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		if (ImGui::Begin("Viewport", nullptr, windowFlags)) {
			ImVec2 avail = ImGui::GetContentRegionAvail();

			ImGui::Image((void*)(uint64_t)Editor->getFinalTexture()->handle(), avail, { 0, 1 }, { 1, 0 });

			s_sceneViewportSize = glm::ivec2((uint32_t)avail.x, (uint32_t)avail.y);
			s_mouseInViewport = ImGui::IsWindowHovered();

			s_viewportFocused = ImGui::IsWindowFocused();

			if (s_mouseInViewport) {
				ImGui::SetNextFrameWantCaptureMouse(false);
			}

			if (s_viewportFocused) {
				ImGui::SetNextFrameWantCaptureKeyboard(false);
			}

			if (s_mouseInViewport && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				ImGui::FocusWindow(ImGui::GetCurrentWindow());
			}
		} else {
			s_sceneViewportSize = glm::ivec2(1, 1);
			s_mouseInViewport = false;
			s_viewportFocused = false;
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorWindow::drawWindows() {
		Ref<Scene>& activeScene = SceneManager::getActiveScene();
		bool sceneOpen = activeScene != nullptr;
		ImGui::BeginDisabled(!sceneOpen);
		drawViewport();

		if (EditorWindows.demo) {
			ImGui::ShowDemoWindow(&EditorWindows.demo);
		}
		m_inspectorPanel.draw(activeScene, &m_hierarchyPanel);
		m_logPanel.draw();
		DebugWindow::draw();

		m_assetBrowserPanel.draw();
		m_scenePanel.draw();

		ProfilerWindow::draw();

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 1));

		m_hierarchyPanel.draw();
		ImGui::EndDisabled();
	}

	void EditorWindow::update(Timestep ts) {
		m_hierarchyPanel.update(ts);


		//if (Keyboard::keyDown(Key::N)) {
		//	offset++;
		//}
		//if (Keyboard::keyDown(Key::M)) {
		//	offset--;
		//}
		//Log::info("Offset: {}", offset);
	}

	void EditorWindow::onImGuiRender() {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		float titlebarYOffset = App->getWindow()->isMaximized() ? 8.0f : 0.0f;
		const float titleBarHeight = DPI::getScale(44.0f);

		drawTitlebar(viewport->Pos, ImVec2(viewport->Size.x, titleBarHeight + titlebarYOffset), viewport->ID, titleBarHeight);
		drawEditor(ImVec2(viewport->Pos.x, viewport->Pos.y + titleBarHeight + titlebarYOffset), ImVec2(viewport->Size.x, viewport->Size.y - titleBarHeight - titlebarYOffset), viewport->ID, titleBarHeight);

		ImGui::PopStyleVar(2);

		drawWindows();
	}

	void EditorWindow::fixedUpdate(Timestep ts) {

	}

	void EditorWindow::destroy() {

	}

	glm::ivec2 EditorWindow::getSceneViewportSize() const {
		return s_sceneViewportSize;
	}

	bool EditorWindow::isViewportFocused() const {
		return s_viewportFocused;
	}

	bool EditorWindow::isMouseInViewport() const {
		return s_mouseInViewport;
	}
}