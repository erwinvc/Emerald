#include "eepch.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"

#include "graphics/Window.h"
#include "iconsFontAwesome.h"
#include "imguiManager.h"
#include "imguiProfiler/IconsFontAwesome4.h"
#include "implot/implot.h"

namespace emerald {
	ImGuiContext* context;

	std::unordered_map<ImGUIFont, ImFont*> fonts;

	static const char* ImGui_ImplGlfw_GetClipboardText(void* user_data) {
		return glfwGetClipboardString((GLFWwindow*)user_data);
	}

	static void ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text) {
		glfwSetClipboardString((GLFWwindow*)user_data, text);
	}

	void applyEmeraldTheme() {
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		Color transparent = Color(0x00000000);

		colors[ImGuiCol_Text] = Color(0xD6D6D6FF);
		colors[ImGuiCol_TextDisabled] = Color(0xafafafFF);
		colors[ImGuiCol_WindowBg] = Color(0x1F1F1FFF);
		colors[ImGuiCol_ChildBg] = Color(0x1B1B1BFF);
		colors[ImGuiCol_PopupBg] = Color(0x303030FF);
		colors[ImGuiCol_Border] = Color(0x3D3D3DFF);
		colors[ImGuiCol_BorderShadow] = Color(0x00000080); //Todo?
		colors[ImGuiCol_FrameBg] = Color(0x0C0C0C8A);
		colors[ImGuiCol_FrameBgHovered] = Color(0x3D3D3DFF);
		colors[ImGuiCol_FrameBgActive] = Color(0x4A4A4AFF);
		colors[ImGuiCol_TitleBg] = Color(0x282828FF);
		colors[ImGuiCol_TitleBgActive] = Color(0x282828FF);
		//colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = Color(0x282828FF);
		colors[ImGuiCol_ScrollbarBg] = transparent;
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = Color(0xD6D6D6FF);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.29f, 0.29f, 0.29f, 1.0f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
		colors[ImGuiCol_Header] = ImVec4(0.29f, 0.29f, 0.29f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.29f, 0.29f, 0.29f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
		colors[ImGuiCol_Separator] = Color(0x1F1F1FFF);
		colors[ImGuiCol_SeparatorHovered] = Color(0x3D3D3DFF);
		colors[ImGuiCol_SeparatorActive] = Color(0x383838FF);
		colors[ImGuiCol_ResizeGrip] = Color(0x1F1F1FFF);
		colors[ImGuiCol_ResizeGripHovered] = Color(0x3D3D3DFF);
		colors[ImGuiCol_ResizeGripActive] = Color(0x383838FF);
		colors[ImGuiCol_TabHovered] = Color(0x3D3D3DFF);
		colors[ImGuiCol_Tab] = Color(0x1F1F1FFF);
		colors[ImGuiCol_TabSelected] = Color(0x383838FF);
		colors[ImGuiCol_TabSelectedOverline] = transparent;
		colors[ImGuiCol_TabDimmed] = Color(0x1F1F1FFF);
		colors[ImGuiCol_TabDimmedSelected] = Color(0x383838FF);
		colors[ImGuiCol_TabDimmedSelectedOverline] = transparent;
		colors[ImGuiCol_DockingPreview] = Color(0x6E6E6EFF);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
		colors[ImGuiCol_PlotLinesHovered] = Color(0xD6D6D6FF);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
		colors[ImGuiCol_PlotHistogramHovered] = Color(0xD6D6D6FF);
		//colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		//colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
		//colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
		//colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		//colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextLink] = colors[ImGuiCol_HeaderActive];
		colors[ImGuiCol_TextSelectedBg] = Color(0x3D3D3DFF);
		//colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.5f, 0.0f, 1.0f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);

#if 0
		for (int i = 0; i <= ImGuiCol_COUNT; i++) {
			ImVec4& col = colors[i];
			float H, S, V;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

			if (S < 0.1f) {
				V = 1.0f - V;
			}
			ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
			if (col.w < 1.00f) {
				col.w *= 0.5f;
			}
		}
#endif

		style->WindowPadding = ImVec2(4, 4);
		style->SeparatorTextPadding = ImVec2(20, 0);
		style->SeparatorTextBorderSize = 5;
		style->TabRounding = 3;
		style->DockingSeparatorSize = 3;
		style->TabBarBorderSize = 1;
	}

	void ImGuiManager::initialize(Ref<Window> window) {
		IMGUI_CHECKVERSION();
		context = ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.IniFilename = NULL;
		//io.ConfigDockingWithShift = true;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigDockingWithShift = true;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
		io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
		io.ClipboardUserData = window->handle();

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		ImGui_ImplGlfw_InitForOpenGL(window->handle(), true);
		ImGui_ImplOpenGL3_Init("#version 410");

		ImGui::GetIO().Fonts->AddFontDefault();

		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 15.0f;
	
		const ImWchar segmdl2Ranges[] = { 0xE700, 0xF624, 0 };
		const ImWchar awesomeRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		const ImWchar profilerAwesomeRanges[] = { P_ICON_MIN_FA, P_ICON_MAX_FA, 0 };

		fonts[ImGUIFont::INTER] = io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/Inter.ttc", 18.0f, NULL);
		io.Fonts->AddFontFromFileTTF("res/fonts/fontawesome-webfont.ttf", 15.0f, &config, profilerAwesomeRanges); //Profiler lib

		config.MergeMode = true;
		config.GlyphOffset.y = 4;
		fonts[ImGUIFont::CONSOLAS] =  io.Fonts->AddFontFromFileTTF("res/fonts/Consolas.ttf", 16.0f, NULL);
		io.Fonts->AddFontFromFileTTF("res/fonts/SegMDL2.ttf", 16.0f, &config, segmdl2Ranges);
		
		config.MergeMode = false;
		fonts[ImGUIFont::SEGOE] = io.Fonts->AddFontFromFileTTF("res/fonts/SegMDL2.ttf", 10.0f, NULL, segmdl2Ranges);
		fonts[ImGUIFont::AWESOME_R] = io.Fonts->AddFontFromFileTTF("res/fonts/fa-regular-400.ttf", 16.0f, &config, awesomeRanges);
		fonts[ImGUIFont::AWESOME_S] = io.Fonts->AddFontFromFileTTF("res/fonts/fa-solid-900.ttf", 16.0f, &config, awesomeRanges);

		io.Fonts->Build();

		applyEmeraldTheme();

		Log::info("[ImGui] initialized");
	}

	void ImGuiManager::pushFont(ImGUIFont font) {
		ImGui::PushFont(fonts[font]);
	}

	void ImGuiManager::popFont() {
		ImGui::PopFont();
	}

	void ImGuiManager::begin() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	void ImGuiManager::end() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		GLFWwindow* backup = glfwGetCurrentContext();
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
		glfwMakeContextCurrent(backup);
	}

	void ImGuiManager::shutdown() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext(context);
	}

}
namespace ImGui{
	//Custom widgets
	void ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags flags) {
		ImGuiWindowClass window_class;
		window_class.DockNodeFlagsOverrideSet = flags;
		ImGui::SetNextWindowClass(&window_class);
	}
	void DrawGradientBackgroundForWindow(GradientDirection gradientDirection) {
		float size = 50;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 p_min = ImGui::GetWindowPos();
		ImVec2 p_max = ImVec2(p_min.x + ImGui::GetWindowWidth(), p_min.y + ImGui::GetWindowHeight());

		ImU32 color_start = IM_COL32(0, 0, 0, 0);       // Transparent
		ImU32 color_end = IM_COL32(0, 0, 0, 155);       // Semi-transparent black

		switch (gradientDirection) {
			case RIGHT:
				p_min.x += (ImGui::GetWindowWidth() - size);
				p_max.x = p_min.x + size;
				draw_list->AddRectFilledMultiColor(p_min, p_max, color_start, color_end, color_end, color_start);
				break;
			case LEFT:
				p_max.x = p_min.x + size;
				draw_list->AddRectFilledMultiColor(p_min, p_max, color_end, color_start, color_start, color_end);
				break;
			case TOP:
				p_max.y = p_min.y + size;
				draw_list->AddRectFilledMultiColor(p_min, p_max, color_end, color_end, color_start, color_start);
				break;
			case BOTTOM:
				p_min.y += (ImGui::GetWindowHeight() - size);
				p_max.y = p_min.y + size;
				draw_list->AddRectFilledMultiColor(p_min, p_max, color_start, color_start, color_end, color_end);
				break;
		}
	}

	bool ColoredButton(const char* label, const ImVec2& size_arg, ImU32 text_color, ImU32 bg_color_1, ImU32 bg_color_2) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		ImGuiButtonFlags flags = ImGuiButtonFlags_None;
		if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

		// Render
		const bool is_gradient = bg_color_1 != bg_color_2;
		if (held || hovered) {
			// Modify colors (ultimately this can be prebaked in the style)
			float h_increase = (held && hovered) ? 0.02f : 0.02f;
			float v_increase = (held && hovered) ? 0.20f : 0.07f;

			ImVec4 bg1f = ImGui::ColorConvertU32ToFloat4(bg_color_1);
			ImGui::ColorConvertRGBtoHSV(bg1f.x, bg1f.y, bg1f.z, bg1f.x, bg1f.y, bg1f.z);
			bg1f.x = ImMin(bg1f.x + h_increase, 1.0f);
			bg1f.z = ImMin(bg1f.z + v_increase, 1.0f);
			ImGui::ColorConvertHSVtoRGB(bg1f.x, bg1f.y, bg1f.z, bg1f.x, bg1f.y, bg1f.z);
			bg_color_1 = ImGui::GetColorU32(bg1f);
			if (is_gradient) {
				ImVec4 bg2f = ImGui::ColorConvertU32ToFloat4(bg_color_2);
				ImGui::ColorConvertRGBtoHSV(bg2f.x, bg2f.y, bg2f.z, bg2f.x, bg2f.y, bg2f.z);
				bg2f.z = ImMin(bg2f.z + h_increase, 1.0f);
				bg2f.z = ImMin(bg2f.z + v_increase, 1.0f);
				ImGui::ColorConvertHSVtoRGB(bg2f.x, bg2f.y, bg2f.z, bg2f.x, bg2f.y, bg2f.z);
				bg_color_2 = ImGui::GetColorU32(bg2f);
			} else {
				bg_color_2 = bg_color_1;
			}
		}
		ImGui::RenderNavHighlight(bb, id);

#if 0
		// V1 : faster but prevents rounding
		window->DrawList->AddRectFilledMultiColor(bb.Min, bb.Max, bg_color_1, bg_color_1, bg_color_2, bg_color_2);
		if (g.Style.FrameBorderSize > 0.0f)
			window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_Border), 0.0f, 0, g.Style.FrameBorderSize);
#endif

		// V2
		int vert_start_idx = window->DrawList->VtxBuffer.Size;
		window->DrawList->AddRectFilled(bb.Min, bb.Max, bg_color_1, g.Style.FrameRounding);
		int vert_end_idx = window->DrawList->VtxBuffer.Size;
		if (is_gradient)
			ImGui::ShadeVertsLinearColorGradientKeepAlpha(window->DrawList, vert_start_idx, vert_end_idx, bb.Min, bb.GetBL(), bg_color_1, bg_color_2);
		if (g.Style.FrameBorderSize > 0.0f)
			window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_Border), g.Style.FrameRounding, 0, g.Style.FrameBorderSize);

		if (g.LogEnabled)
			ImGui::LogSetNextTextDecoration("[", "]");
		ImGui::PushStyleColor(ImGuiCol_Text, text_color);
		ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		ImGui::PopStyleColor();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
		return pressed;
	}

	void ItemRowsBackground(float lineHeight, const ImColor& color) {
		auto* drawList = ImGui::GetWindowDrawList();
		const auto& style = ImGui::GetStyle();

		if (lineHeight < 0) {
			lineHeight = ImGui::GetTextLineHeight();
		}
		lineHeight += style.ItemSpacing.y;

		float scrollOffsetH = ImGui::GetScrollX();
		float scrollOffsetV = ImGui::GetScrollY();
		float scrolledOutLines = floorf(scrollOffsetV / lineHeight);
		scrollOffsetV -= lineHeight * scrolledOutLines;

		ImVec2 clipRectMin(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		ImVec2 clipRectMax(clipRectMin.x + ImGui::GetWindowWidth(), clipRectMin.y + ImGui::GetWindowHeight());

		if (ImGui::GetScrollMaxX() > 0) {
			clipRectMax.y -= style.ScrollbarSize;
		}

		drawList->PushClipRect(clipRectMin, clipRectMax);

		bool isOdd = (static_cast<int>(scrolledOutLines) % 2) == 0;

		float yMin = clipRectMin.y - scrollOffsetV + ImGui::GetCursorPosY();
		float yMax = clipRectMax.y - scrollOffsetV + lineHeight;
		float xMin = clipRectMin.x + scrollOffsetH + ImGui::GetWindowContentRegionMin().x;
		float xMax = clipRectMin.x + scrollOffsetH + ImGui::GetWindowContentRegionMax().x;

		for (float y = yMin; y < yMax; y += lineHeight, isOdd = !isOdd) {
			if (isOdd) {
				drawList->AddRectFilled({ xMin, y - style.ItemSpacing.y }, { xMax, y + lineHeight }, color);
			}
		}

		drawList->PopClipRect();
	}

}