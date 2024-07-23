#include "editorWindow.h"
#include "imgui.h"
#include <imgui_internal.h>
#include <GLFW/glfw3.h>
#include "graphics/window.h"
#include "ui/iconsFontAwesome.h"
#include "ui/imguiManager.h"
#include "ui/iconsFontSegoeMDL2.h"

namespace emerald {
	static bool m_TitleBarHovered = false;
	void EditorWindow::initialize() {
		glfwSetTitlebarHitTestCallback(App->getWindow()->getHandle(), [](GLFWwindow* window, int x, int y, int* hit) {
			*hit = m_TitleBarHovered;
			});
	}

	enum class CursorMode {
		Normal = 0,
		Hidden = 1,
		Locked = 2
	};

	typedef enum class MouseButton : uint16_t {
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Left = Button0,
		Right = Button1,
		Middle = Button2
	} Button;

	typedef enum class KeyCode : uint16_t {
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	bool IsKeyDown(KeyCode keycode) {
		GLFWwindow* windowHandle = App->getWindow()->getHandle();
		int state = glfwGetKey(windowHandle, (int)keycode);
		return state == GLFW_PRESS;/* || state == GLFW_REPEAT;*/
	}

	bool IsMouseButtonDown(MouseButton button) {
		GLFWwindow* windowHandle = App->getWindow()->getHandle();
		int state = glfwGetMouseButton(windowHandle, (int)button);
		return state == GLFW_PRESS;
	}

	void drawTitlebar(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight) {
		const bool maximized = App->getWindow()->isMaximized();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollbar;
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, maximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize(size);
		ImGui::SetNextWindowViewport(viewportID);

		ImGui::Begin("TitleBarWindow", nullptr, window_flags);
		imGuiManager::pushFont(imGuiManager::ImGUIFont::SEGOE);

		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;
		const float titleBarButtonSize = 46;
		const float buttonsAreaWidth = titleBarButtonSize * 4;
		const ImVec2 buttonSize = ImVec2(titleBarButtonSize, 46);

		const float w = ImGui::GetContentRegionAvail().x;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + (maximized ? 2.0f : 0.0f)));
		ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth(), titlebarHeight });
		ImGui::BeginDisabled(true);
		ImGui::Button((const char*)SEGOE_MDL2_ICON_COMPONENT, buttonSize);
		m_TitleBarHovered = ImGui::IsItemHovered();

		ImGui::EndDisabled();
		ImGui::InvisibleButton("", ImVec2(w - buttonsAreaWidth + 2, titlebarHeight));
		m_TitleBarHovered |= ImGui::IsItemHovered();

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
		ImGui::EndHorizontal();
		imGuiManager::popFont();

		ImGui::PopStyleVar(2);

		ImGui::End();
	}

	void applyNodeFlagsToNextWindow(ImGuiDockNodeFlags flags) {
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = flags;
		ImGui::SetNextWindowClass(&window_class);
	}

	void drawMenuBar() {
		if (ImGui::BeginMenuBar()) {
			//m_MenubarCallback();
			if (ImGui::BeginMenu("Edit")) {
				ImGui::MenuItem("Dummy");
				ImGui::MenuItem("Dummy");
				ImGui::MenuItem("Dummy");
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("File")) {}
			if (ImGui::MenuItem("View")) {}
			if (ImGui::MenuItem("Window")) {}
			ImGui::EndMenuBar();
		}
	}
	static int offset = 0;

	void drawAppArea(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));

		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize(size);
		ImGui::SetNextWindowViewport(viewportID);

		ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
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
			ImGui::DockBuilderDockWindow("Hierarchy", left);
			ImGui::DockBuilderDockWindow("Log", down);
			ImGui::DockBuilderDockWindow("Viewport", dockspace_main_id);
			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGui::DockSpace(dockspace_id);
		drawMenuBar();
		ImGui::End();
	}

	void drawWindows() {
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus;

		applyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoDockingOverMe);
		ImGui::Begin("Viewport", nullptr, windowFlags);
		ImGui::End();

		applyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::Begin("Inspector", nullptr, windowFlags);
		ImGui::End();

		applyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::Begin("Log", nullptr, windowFlags);
		ImGui::End();

		applyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::Begin("Hierarchy", nullptr, windowFlags);
		ImGui::End();
	}

	void EditorWindow::update(Timestep ts) {
		/*
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_MenuBar;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 16);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4);
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 2);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 4);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 8);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
		ImGui::PopStyleVar();


		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspace_id = ImGui::GetID("VulkanAppDockspace");
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		ImGui::PopStyleVar();

		//if (m_MenubarCallback) {
		if (ImGui::BeginMenuBar()) {
			//m_MenubarCallback();
			if (ImGui::BeginMenu("Edit")) {
				ImGui::MenuItem("Dummy");
				ImGui::MenuItem("Dummy");
				ImGui::MenuItem("Dummy");
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("File")) {}
			if (ImGui::MenuItem("View")) {}
			if (ImGui::MenuItem("Window")) {}
			ImGui::EndMenuBar();
		}

		ImGui::ShowDemoWindow(nullptr);
		ImGuiWindowFlags window_flags2 = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;
		ImGui::Begin("Viewport", nullptr, window_flags2);
		ImGui::End();
		ImGui::Begin("Inspector", nullptr, window_flags2);
		ImGui::End();
		ImGui::Begin("Log", nullptr, window_flags2);
		ImGui::End();
		ImGui::Begin("Hierarchy", nullptr, window_flags2);
		ImGui::End();
		//}

		static auto first_time = true;
		if (first_time) {
			first_time = false;
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
			// Main node should cover entire window
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
			// get id of main dock space area
			ImGuiID dockspace_main_id = dockspace_id;
			// Create a dock node for the right docked window
			ImGuiID right = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_main_id);
			ImGuiID left = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Left, 0.25f, nullptr, &dockspace_main_id);
			ImGuiID down = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_main_id);

			ImGui::DockBuilderDockWindow("Inspector", right);
			ImGui::DockBuilderDockWindow("Hierarchy", left);
			ImGui::DockBuilderDockWindow("Log", down);
			ImGui::DockBuilderDockWindow("Viewport", dockspace_main_id);
			ImGui::DockBuilderFinish(dockspace_id);
		}
		ImGui::PopStyleVar(8);

		//ImGui::End();
		ImGui::End();


		*/

		static bool p = false;
		static bool p2 = false;

		if (!IsKeyDown(KeyCode::KPAdd)) {
			p = true;
		}

		if (p && IsKeyDown(KeyCode::KPAdd)) {
			p = false;
			offset++;
		}

		if (!IsKeyDown(KeyCode::KPSubtract)) {
			p2 = true;
		}

		if (p2 && IsKeyDown(KeyCode::KPSubtract)) {
			p2 = false;
			offset--;
		}

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

		float titlebarYOffset = App->getWindow()->isMaximized() ? 8.0f : 0.0f;
		const float titleBarHeight = 44;

		drawTitlebar(viewport->Pos, ImVec2(viewport->Size.x, titleBarHeight + titlebarYOffset), viewport->ID, titleBarHeight);
		drawAppArea(ImVec2(viewport->Pos.x, viewport->Pos.y + titleBarHeight + titlebarYOffset), ImVec2(viewport->Size.x, viewport->Size.y - titleBarHeight - titlebarYOffset), viewport->ID, titleBarHeight);

		ImGui::PopStyleVar(2);

		drawWindows();

		applyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		ImGui::ShowDemoWindow(nullptr);
	}

	void EditorWindow::fixedUpdate(Timestep ts) {

	}

	void EditorWindow::destroy() {

	}
}