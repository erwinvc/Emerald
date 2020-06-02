#pragma once

class Window;

class ImGuiManager : public Singleton<ImGuiManager> {
private:
	static ImFont* g_bigFont;
	static ImFont* g_smallFont;
	bool m_initialized = false;
	float m_time = 0;
public:
	ImGuiManager() {}
	~ImGuiManager() {}
	friend Singleton;

	bool IsInitialized() { return m_initialized; }
	void Initialize(Window* window);
	void ApplyStyle();
	void Begin();
	void End();

	static int IPFilter(ImGuiTextEditCallbackData* data) {
		ImWchar c = data->EventChar;
		if (c >= '0' && c < '9') return 0;
		if (c == '.' || c == ':') return 0;
		return 1;
	}
};

static ImGuiManager* GetImGuiManager() { return ImGuiManager::GetInstance(); }

class UI {
private:
	static char buffer[];
	static void Prepare(const String_t name) {
		ImGui::AlignTextToFramePadding();

		ImGui::Text(name);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
	}
	static void Finish() {
		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

public:
	static bool BeginWindow(const String_t name, ImVec2 size = ImVec2(0, 0), bool* open = nullptr);

	static void EndWindow() {
		ImGui::End();
	}

	static void Begin() {
		if (ImGui::GetColumnsCount() != 2) ImGui::Columns(2);
	}

	static void End() {
		if (ImGui::GetColumnsCount() != 1) ImGui::Columns(1);
	}

	static void Header(const String_t name) {
		Prepare(name);
		Finish();
	}

	static bool Button(const String_t name) {
		Prepare(name);
		bool toRet = ImGui::Button(Format_b(buffer, 0x100, "##%s", name), ImVec2(42, 21));
		Finish();
		return toRet;
	}
	
	static void Text(const String_t name, const String& text) {
		Prepare(name);
		ImGui::LabelText(Format_b(buffer, 0x100, "##%s", name), text.c_str());
		Finish();
	}

	static void Bool(const String_t name, bool* value) {
		Prepare(name);
		ImGui::Checkbox(Format_b(buffer, 0x100, "##%s", name), value);
		Finish();
	}

	static void Separator() {
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(0, 4));
		ImGui::Separator();
	}

	static void Dummy(int height = 4) {
		ImGui::Dummy(ImVec2(0, (float)height));
	}

	static void Int(const String_t name, int* value, int min = -10, int max = 10) {
		Prepare(name);
		ImGui::SliderInt(Format_b(buffer, 0x100, "##%s", name), value, min, max);
		Finish();
	}

	static void Float(const String_t name, float* value, float min = -1.0f, float max = 1.0f) {
		Prepare(name);
		ImGui::SliderFloat(Format_b(buffer, 0x100, "##%s", name), value, min, max);
		Finish();
	}

	static void Vec2(const String_t name, glm::vec2* value, float min = -1.0f, float max = 1.0f) {
		Prepare(name);
		ImGui::SliderFloat2(Format_b(buffer, 0x100, "##%s", name), (float*)value, min, max);
		Finish();
	}

	static void Vec3(const String_t name, glm::vec3* value, float min = -1.0f, float max = 1.0f) {
		Prepare(name);
		ImGui::SliderFloat3(Format_b(buffer, 0x100, "##%s", name), (float*)value, min, max);
		Finish();
	}

	static void Vec4(const String_t name, glm::vec4* value, float min = -1.0f, float max = 1.0f) {
		Prepare(name);
		ImGui::SliderFloat4(Format_b(buffer, 0x100, "##%s", name), (float*)value, min, max);
		Finish();
	}

	static void Color3(const String_t name, Color* value) {
		Prepare(name);
		ImGui::ColorEdit3(Format_b(buffer, 0x100, "##%s", name), (float*)value, ImGuiColorEditFlags_NoInputs);
		Finish();
	}

	static void Color4(const String_t name, Color* value) {
		Prepare(name);
		ImGui::ColorEdit4(Format_b(buffer, 0x100, "##%s", name), (float*)value, ImGuiColorEditFlags_NoInputs);
		Finish();
	}

	static bool Combo(const String_t name, int* item, String_t const items[], int itemCount) {
		Prepare(name);
		bool toRet = ImGui::Combo(Format_b(buffer, 0x100, "##%s", name), item, items, itemCount);
		Finish();
		return toRet;
	}
};

namespace ImGui {
	static void Tooltip(String_t tooltip) {
		if (ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.5f) {
			ImGui::SetTooltip(tooltip);
		}
	}
}