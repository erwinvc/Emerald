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

	static int IPFilter(ImGuiTextEditCallbackData* data);
	static int PathFilter(ImGuiTextEditCallbackData* data);
};

inline ImGuiManager* GetImGuiManager() { return ImGuiManager::GetInstance(); }

class UI {
private:
	static char buffer[];
	static void Prepare(const String_t name);
	static void Finish();

public:
	static bool BeginWindow(const String_t name, ImVec2 size = ImVec2(0, 0), bool* open = nullptr);
	static void EndWindow();
	static void Begin();
	static void End();
	static void Header(const String_t name);
	static void Text(const String_t name, const String& text);
	static void Bool(const String_t name, bool* value);
	static void Separator();
	static void Dummy(int height = 4);
	static void Int(const String_t name, int* value, int min = -10, int max = 10);
	static void Float(const String_t name, float* value, float min = -1.0f, float max = 1.0f);
	static void Vec2(const String_t name, glm::vec2* value, float min = -1.0f, float max = 1.0f);
	static void Vec3(const String_t name, glm::vec3* value, float min = -1.0f, float max = 1.0f);
	static void Vec4(const String_t name, glm::vec4* value, float min = -1.0f, float max = 1.0f);
	static void Color3(const String_t name, Color* value);
	static void Color4(const String_t name, Color* value);
	static bool Combo(const String_t name, int* item, String_t const items[], int itemCount);
};

namespace ImGui {
	 void Tooltip(String_t tooltip);
	 void BeginDisable(bool disabled);
	 void EndDisable(bool disabled);
}