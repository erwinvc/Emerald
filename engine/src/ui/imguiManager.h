#pragma once
#include "imgui.h"

namespace emerald::imGuiManager {
	class emerald::Window;

	enum ImGUIFont {
		DEFAULT,
		SEGOE,
		AWESOME_R, //regular
		AWESOME_S //solid
	};

	void pushFont(ImGUIFont font);
	void popFont();

	bool ColoredButton(const char* label, const ImVec2& size_arg, ImU32 text_color, ImU32 bg_color_1, ImU32 bg_color_2);

	void initialize(emerald::Window* window);
	void begin();
	void end();
	void shutdown();
}
