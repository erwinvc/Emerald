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

	void IPV(const String& line) { //Invalid property value
		LOG_ERROR("[~bShaders~x] Invalid property value at [~1%d: %s~x]", m_currentLine, line.c_str());
	}

public:
	~ShaderProperties() {
		Clear();
	}

	const vector<ShaderProperty*>& GetProperties() const { return m_properties; }

	void Parse(const String& src) {
		vector<String> split = Utils::Split(src, "\n");
		if (split.size() < 1) return;
		vector<String> splitVersion = Utils::Split(split[0], "//");
		if (splitVersion.size() == 1 || splitVersion[1].compare("material") != 0) {
			return;
		}

		for (String& line : split) {
			m_currentLine++;
			Utils::LTrim(line);
			if (line._Starts_with("uniform")) {
				vector<String> splitUniform = Utils::Split(line, "//");

				char cType[64] = { 0 };
				char cUniform[128] = { 0 };

				char cPropertyTypeAndName[64] = { 0 };
				//char cPropertyName[64] = { 0 };
				char cPropertyValue[32] = { 0 };

				if (!sscanf(line.c_str(), "uniform %s %s;", &cType, &cUniform)) {
					LOG_ERROR("[~bShaders~x] Failed to retrieve uniform type and name from [~1%s~x]", line.c_str());
				}

				String type = cType;
				if (type.compare("mat3") == 0 || type.compare("mat4") == 0) continue;
				if (splitUniform.size() != 2) LOG_ERROR("[~bShaders~x] Wrong uniform definition [~1%s~x]", line.c_str());
				if (!sscanf(splitUniform[1].c_str(), "(%[^)]) = (%[^)])", &cPropertyTypeAndName, &cPropertyValue)) {
					LOG_ERROR("[~bShaders~x] Failed to retrieve properly definition from [~1%s~x]", line.c_str());
				}

				String propertyTypeAndName = cPropertyTypeAndName;
				vector<String> typeAndNameSplit = Utils::Split(propertyTypeAndName, ",");
				if (typeAndNameSplit.size() != 2) IPV(line);
				String propertyType = typeAndNameSplit[0];
				String propertyName = typeAndNameSplit[1];
				String propertyValue = cPropertyValue;

				vector<String> propertyValueSplit = Utils::Split(propertyValue, ",");
				for (String& s : propertyValueSplit) Utils::Trim(s);

				String uniform = cUniform;
				uniform = uniform.substr(0, uniform.size() - 1); //Remove ;
				uniform = uniform.substr(0, uniform.find_first_of('[')); //Remove array definitions
				if (propertyType.compare("TEXTURE") == 0) {
					if (propertyValueSplit.size() != 2) LOG_ERROR("[~bShaders~x] Invalid property value at [~1%s~x]", line.c_str());
					ShaderPropertyTexture* prop = NEW(ShaderPropertyTexture());
					prop->m_type = ShaderPropertyType::TEXTURE;
					prop->m_uniform = uniform;
					prop->m_name = propertyName;
					if (propertyValueSplit[0].compare("null") == 0) {
						prop->m_default = ShaderPropertyTextureType::INVALID;
					} else if (propertyValueSplit[0].compare("white") == 0) {
						prop->m_default = ShaderPropertyTextureType::WHITE;
					} else if (propertyValueSplit[0].compare("black") == 0) {
						prop->m_default = ShaderPropertyTextureType::BLACK;
					} else if (propertyValueSplit[0].compare("normal") == 0) {
						prop->m_default = ShaderPropertyTextureType::NORMAL;
					} else if (propertyValueSplit[0].compare("transparent") == 0) {
						prop->m_default = ShaderPropertyTextureType::TRANSPARENT;
					} else IPV(line);

					if (!sscanf(propertyValueSplit[1].c_str(), "%d", &prop->m_slot)) IPV(line);
					m_properties.push_back(prop);

				} else if (propertyType.compare("FLOAT") == 0) {
					if (propertyValueSplit.size() == 1) {
						ShaderPropertyFloat* prop = NEW(ShaderPropertyFloat());
						prop->m_type = ShaderPropertyType::FLOAT;
						prop->m_uniform = uniform;
						prop->m_name = propertyName;
						if (!sscanf(propertyValueSplit[0].c_str(), "%f", &prop->m_default)) IPV(line);
						m_properties.push_back(prop);
					} else if (propertyValueSplit.size() == 3) {
						ShaderPropertyRange* prop = NEW(ShaderPropertyRange());
						prop->m_type = ShaderPropertyType::RANGE;
						prop->m_uniform = uniform;
						prop->m_name = propertyName;
						if (!sscanf(propertyValueSplit[0].c_str(), "%f", &prop->m_default)) IPV(line);
						if (!sscanf(propertyValueSplit[1].c_str(), "%f", &prop->m_min)) IPV(line);
						if (!sscanf(propertyValueSplit[2].c_str(), "%f", &prop->m_max)) IPV(line);
						m_properties.push_back(prop);

					} else IPV(line);

				} else if (propertyType.compare("VECTOR2") == 0) {
					if (propertyValueSplit.size() != 2) IPV(line);
					ShaderPropertyVec2* prop = NEW(ShaderPropertyVec2());
					prop->m_type = ShaderPropertyType::VECTOR2;
					prop->m_uniform = uniform;
					prop->m_name = propertyName;
					if (!sscanf(propertyValueSplit[0].c_str(), "%f", &prop->m_default.x)) IPV(line);
					if (!sscanf(propertyValueSplit[1].c_str(), "%f", &prop->m_default.y)) IPV(line);
					m_properties.push_back(prop);

				} else if (propertyType.compare("VECTOR3") == 0) {
					if (propertyValueSplit.size() != 3) IPV(line);
					ShaderPropertyVec3* prop = NEW(ShaderPropertyVec3());
					prop->m_type = ShaderPropertyType::VECTOR3;
					prop->m_uniform = uniform;
					prop->m_name = propertyName;
					if (!sscanf(propertyValueSplit[0].c_str(), "%f", &prop->m_default.x)) IPV(line);
					if (!sscanf(propertyValueSplit[1].c_str(), "%f", &prop->m_default.y)) IPV(line);
					if (!sscanf(propertyValueSplit[2].c_str(), "%f", &prop->m_default.z)) IPV(line);
					m_properties.push_back(prop);

				} else if (propertyType.compare("VECTOR4") == 0) {
					if (propertyValueSplit.size() != 4) IPV(line);
					ShaderPropertyVec4* prop = NEW(ShaderPropertyVec4());
					prop->m_type = ShaderPropertyType::VECTOR4;
					prop->m_uniform = uniform;
					prop->m_name = propertyName;
					if (!sscanf(propertyValueSplit[0].c_str(), "%f", &prop->m_default.x)) IPV(line);
					if (!sscanf(propertyValueSplit[1].c_str(), "%f", &prop->m_default.y)) IPV(line);
					if (!sscanf(propertyValueSplit[2].c_str(), "%f", &prop->m_default.z)) IPV(line);
					if (!sscanf(propertyValueSplit[3].c_str(), "%f", &prop->m_default.w)) IPV(line);
					m_properties.push_back(prop);

				} else if (propertyType.compare("COLOR") == 0) {
					if (propertyValueSplit.size() == 3) {
						ShaderPropertyRGB* prop = NEW(ShaderPropertyRGB());
						prop->m_type = ShaderPropertyType::RGB;
						prop->m_uniform = uniform;
						prop->m_name = propertyName;
						if (!sscanf(propertyValueSplit[0].c_str(), "%f", &prop->m_default.R)) IPV(line);
						if (!sscanf(propertyValueSplit[1].c_str(), "%f", &prop->m_default.G)) IPV(line);
						if (!sscanf(propertyValueSplit[2].c_str(), "%f", &prop->m_default.B)) IPV(line);
						m_properties.push_back(prop);

					} else if (propertyValueSplit.size() == 4) {
						ShaderPropertyRGBA* prop = NEW(ShaderPropertyRGBA());
						prop->m_type = ShaderPropertyType::RGBA;
						prop->m_uniform = uniform;
						prop->m_name = propertyName;
						if (!sscanf(propertyValueSplit[0].c_str(), "%f", &prop->m_default.R)) IPV(line);
						if (!sscanf(propertyValueSplit[1].c_str(), "%f", &prop->m_default.G)) IPV(line);
						if (!sscanf(propertyValueSplit[2].c_str(), "%f", &prop->m_default.B)) IPV(line);
						if (!sscanf(propertyValueSplit[3].c_str(), "%f", &prop->m_default.A)) IPV(line);
						m_properties.push_back(prop);

					} else IPV(line);

				} else if (propertyType.compare("BOOL") == 0) {
					if (propertyValueSplit.size() != 1) IPV(line);
					ShaderPropertyBool* prop = NEW(ShaderPropertyBool());
					prop->m_type = ShaderPropertyType::BOOL;
					prop->m_uniform = uniform;
					prop->m_name = propertyName;
					int res = 0;
					if (!sscanf(propertyValueSplit[0].c_str(), "%d", &res)) IPV(line);
					prop->m_default = res == 0 ? false : true;
					m_properties.push_back(prop);
				} else IPV(line);
			}
		}
	}

	void Clear() {
		for (auto& prop : m_properties) {
			DELETE(prop);
		}
		m_properties.clear();
	}
};