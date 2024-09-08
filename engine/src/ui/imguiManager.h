#pragma once
#include "imgui.h"

namespace emerald {
	class Window;

	enum ImGUIFont {
		CONSOLAS,
		SEGOE,
		INTER,
		AWESOME_R, //regular
		AWESOME_S //solid
	};
	class ImGuiManager {
	public:
		static void pushFont(ImGUIFont font);
		static void popFont();

		static void initialize(Ref<Window> window);
		static void begin();
		static void end();
		static void shutdown();

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
	void ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags flags);
	void DrawGradientBackgroundForWindow(GradientDirection gradientDirection, ImU32 color = IM_COL32(0, 0, 0, 155), float size = 50);
	bool ColoredButton(const char* label, const ImVec2& size_arg, ImU32 text_color, ImU32 bg_color_1, ImU32 bg_color_2);
	void ItemRowsBackground(float lineHeight = -1.0f, const ImColor& color = ImColor(20, 20, 20, 64));
	void PushEmeraldPopupStyle();
	void PopEmeraldPopupStyle();
	void BorderSeparator(uint32_t extraYSpacing = 1);
	bool EmeraldButton(const char* label, const ImVec2& size = ImVec2(0, 0));
}