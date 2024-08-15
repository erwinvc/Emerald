#pragma once
#include "imgui.h"

namespace emerald {
	enum ImGUIFont {
		CONSOLAS,
		SEGOE,
		INTER,
		AWESOME_R, //regular
		AWESOME_S //solid
	};
	namespace imGuiManager {
		class emerald::Window;

		void pushFont(ImGUIFont font);
		void popFont();

		bool ColoredButton(const char* label, const ImVec2& size_arg, ImU32 text_color, ImU32 bg_color_1, ImU32 bg_color_2);

		void initialize(Ref<Window> window);
		void begin();
		void end();
		void shutdown();
	}
}