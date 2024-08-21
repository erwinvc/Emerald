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
	void DrawGradientBackgroundForWindow(GradientDirection gradientDirection);
	bool ColoredButton(const char* label, const ImVec2& size_arg, ImU32 text_color, ImU32 bg_color_1, ImU32 bg_color_2);
}