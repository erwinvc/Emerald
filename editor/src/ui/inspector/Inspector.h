#pragma once
#include "imgui.h"
#include <imgui_internal.h>

namespace emerald::Inspector {
	static bool DragScalarN(const char* label, ImGuiDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags, const char** symbols) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		bool value_changed = false;
		ImGui::BeginGroup();
		ImGui::PushID(label);
		const int spacing = 9; //Label size
		const int spacingCount = 5; //1 for text, 3 for labels and 1 for separator
		ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth() - spacing * spacingCount);
		size_t type_size = ImGui::DataTypeGetInfo(data_type)->Size;
		for (int i = 0; i < components; i++) {
			ImGui::PushID(i);
			if (i > 0)
				ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
			value_changed |= ImGui::DragScalar(symbols[i], data_type, p_data, v_speed, p_min, p_max, format, flags);
			ImGui::PopID();
			ImGui::PopItemWidth();
			p_data = (void*)((char*)p_data + type_size);
		}
		ImGui::PopID();

		ImGui::EndGroup();
		return value_changed;
	}

	static bool DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return ImGui::DragScalar(label, ImGuiDataType_Float, v, v_speed, &v_min, &v_max, format, flags);
	}

	static bool DragFloat2(const char* label, float v[2], const char** symbols, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return DragScalarN(label, ImGuiDataType_Float, v, 2, v_speed, &v_min, &v_max, format, flags, symbols);
	}

	static bool DragFloat3(const char* label, float v[3], const char** symbols, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return DragScalarN(label, ImGuiDataType_Float, v, 3, v_speed, &v_min, &v_max, format, flags, symbols);
	}

	static bool DragFloat4(const char* label, float v[4], const char** symbols, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return DragScalarN(label, ImGuiDataType_Float, v, 4, v_speed, &v_min, &v_max, format, flags, symbols);
	}

}