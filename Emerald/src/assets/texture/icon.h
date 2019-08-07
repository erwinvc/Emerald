#pragma once

class Icon {
private:
	String m_file;
	uint m_width = 0;
	uint m_height = 0;
	byte* m_data;
	GLFWimage m_icon;

	void Load();
public:
	Icon(const String& file) : m_file(Format("res/%s.png", file.c_str())) { Load(); }
	~Icon();

	const GLFWimage* GetImage() const {
		return &m_icon;
	}
};