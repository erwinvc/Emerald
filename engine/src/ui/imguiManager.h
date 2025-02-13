#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#define IMGUI_STRINGIFY(x) #x
#define IMGUI_TOSTRING(x) IMGUI_STRINGIFY(x)
#define UNIQUE_IMGUI_LABEL(label) label "##" IMGUI_TOSTRING(__LINE__) ":" IMGUI_TOSTRING(__COUNTER__)

namespace emerald {
	class Window;

	enum ImGUIFont {
		CONSOLAS,
		SEGOE,
		SEGOE_TITLEBAR,
		INTER,
		INTER_BOLD,
		AWESOME_R, //regular
		AWESOME_S //solid
	};
	class ImGuiManager {
	public:
		static void pushFont(ImGUIFont font);
		static void popFont();

		static void initialize(const Ref<Window>& window);
		static void rebuildFonts();
		static void begin();
		static void end();
		static void shutdown();
		static ImVec4 getSeverityColor(Severity severity);

		static inline const char* generateLabel(const char* baseLabel, uint32_t index, size_t bufferSize = 64) {
			thread_local static std::vector<char> buffer(bufferSize);
			snprintf(buffer.data(), buffer.size(), "%s_%u", baseLabel, index);
			return buffer.data();
		}
	};
}

namespace ImGui {
	//custom widgets

	enum GradientDirection {
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	};

	template<typename T>
	ImGuiID ptrToImGuiID(const T* ptr) {
		return static_cast<ImGuiID>(reinterpret_cast<uintptr_t>(ptr));
	}

	void ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags flags);
	void DrawGradientBackgroundForWindow(GradientDirection gradientDirection, ImU32 color = IM_COL32(0, 0, 0, 155), float size = 50, uint32_t offset = 0);
	bool ColoredButton(const char* label, const ImVec2& size_arg, ImU32 text_color, ImU32 bg_color_1, ImU32 bg_color_2);
	void ItemRowsBackground(float lineHeight = -1.0f, const ImColor& color = ImColor(20, 20, 20, 64));
	void PushEmeraldPopupStyle();
	void PopEmeraldPopupStyle();
	void BorderSeparator(uint32_t extraYSpacing = 1);
	bool EmeraldButton(const char* label, const ImVec2& size = ImVec2(0, 0));
	bool ToggleButton(const char* label, bool* v, const ImVec2& size = ImVec2(0, 0), ImVec4 inactiveColor = ImGui::GetStyleColorVec4(ImGuiCol_Button), ImVec4 activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
	bool ToggleButton(const char* label, bool* v, const ImVec2& size_arg, const char* activeIcon, const char* inactiveIcon);
}