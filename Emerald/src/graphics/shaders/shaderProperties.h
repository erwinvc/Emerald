#pragma once

enum class ShaderPropertyTextureType {
	INVALID,
	WHITE,
	BLACK,
	NORMAL,
	TRANSPARENT
};

enum class ShaderPropertyType {
	RGB,
	RGBA,
	FLOAT,
	RANGE,
	INT,
	BOOL,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	TEXTURE
};

class ShaderProperty {
public:
	String m_name;
	String m_uniform;
	ShaderPropertyType m_type;
};

class ShaderPropertyRGB : public ShaderProperty {
public:
	Color m_default;
};

class ShaderPropertyRGBA : public ShaderProperty {
public:
	Color m_default;
};

class ShaderPropertyFloat : public ShaderProperty {
public:
	float m_default;
};

class ShaderPropertyRange : public ShaderProperty {
public:
	float m_default;
	float m_min;
	float m_max;
};

class ShaderPropertyInt : public ShaderProperty {
public:
	int m_default;
};

class ShaderPropertyBool : public ShaderProperty {
public:
	bool m_default;
};

class ShaderPropertyVec2 : public ShaderProperty {
public:
	glm::vec2 m_default;
};

class ShaderPropertyVec3 : public ShaderProperty {
public:
	glm::vec3 m_default;
};

class ShaderPropertyVec4 : public ShaderProperty {
public:
	glm::vec4 m_default;
};

class ShaderPropertyTexture : public ShaderProperty {
public:
	ShaderPropertyTextureType m_default;
	int m_slot;
};

class ShaderProperties {
private:
	int m_currentLine = 0;
	vector<ShaderProperty*> m_properties;

	void IPV(const String& line);

public:
	~ShaderProperties();

	void Parse(const String& src);
	void Clear();
	
	inline const vector<ShaderProperty*>& GetProperties() const { return m_properties; }
};