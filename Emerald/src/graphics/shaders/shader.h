#pragma once

class Shader {
private:
	ShaderProgram* m_shaderProgram;

	bool m_hasGeometry;
	bool m_hasTessellation;
	String m_name;
	Path m_filePath;

	ShaderUniformBuffer m_uniformBuffer;
	ShaderProperties m_properties;

	GLuint LoadShader(const String& path, GLuint type) {
		GL(uint shader = glCreateShader(type));
		if (!FileSystem::DoesFileExist(path)) LOG_WARN("[~bShaders~x] ~1%s ~xshader ~1%s ~xat ~1%s does not exist", m_name.c_str(), GLUtils::ShaderTypeToString(type), path.c_str());
		String source = FileSystem::ReadFile(path);
		if (source.empty()) {
			LOG_WARN("[~bShaders~x] Failed to load ~1%s ~xshader ~1%s ~xat ~1%s", m_name.c_str(), GLUtils::ShaderTypeToString(type), path.c_str());
			return 0xffffffff;
		}
		String_t sourceCC = source.c_str();

		m_properties.Parse(sourceCC);
		//LOG("%s %d", m_name.c_str(), m_properties.GetProperties().size());

		GL(glShaderSource(shader, 1, &sourceCC, 0));
		GL(glCompileShader(shader));

		GLint result;
		GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE) {
			GLint length;
			GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
			vector<char> error(length);
			GL(glGetShaderInfoLog(shader, length, &length, &error[0]));
			LOG_WARN("[~bShaders~x] Failed to compile %s %s shader with error: \n%s", m_name.c_str(), GLUtils::ShaderTypeToString(type), &error[0]);
			return 0xffffffff;
		}
		LOG("[~bShaders~x] Compiled ~1%s~x %s", m_name.c_str(), GLUtils::ShaderTypeToString(type));
		return shader;
	}

	bool AddShaderToProgram(ShaderProgram* program, const String& shader, GLuint shaderType) {
		GLuint shaderHandle = LoadShader(shader, shaderType);
		if (shaderHandle == 0xffffffff) return true;
		program->AttachShader(shaderHandle);
		return false;
	}

	ShaderProgram* Load() {
		m_properties.Clear();
		ShaderProgram* shaderProgram = new ShaderProgram();
		shaderProgram->CreateProgram();

		bool failed = false;

		String vertexFile = m_filePath.GetFullPathWithoutExtention() + ".vert";
		String fragFile = m_filePath.GetFullPathWithoutExtention() + ".frag";
		failed |= AddShaderToProgram(shaderProgram, vertexFile, GL_VERTEX_SHADER);
		failed |= AddShaderToProgram(shaderProgram, fragFile, GL_FRAGMENT_SHADER);

		if (m_hasGeometry) {
			String geomFile = m_filePath.GetFullPathWithoutExtention() + ".geom";
			failed |= AddShaderToProgram(shaderProgram, geomFile, GL_GEOMETRY_SHADER);
		}

		if (m_hasTessellation) {
			String te = m_filePath.GetFullPathWithoutExtention() + ".tese";
			String tc = m_filePath.GetFullPathWithoutExtention() + ".tesc";
			failed |= AddShaderToProgram(shaderProgram, te, GL_TESS_EVALUATION_SHADER);
			failed |= AddShaderToProgram(shaderProgram, tc, GL_TESS_CONTROL_SHADER);
		}


		if (failed) {
			delete shaderProgram;
			shaderProgram = nullptr;
		} else {
			shaderProgram->LinkAndValidate();
			shaderProgram->DeleteAttachedShaders();
		}

		return shaderProgram;
	}

	Shader(const String& name, const Path& filePath, bool hasGeometry = false, bool hasTessellation = false) : m_shaderProgram(nullptr), m_hasGeometry(hasGeometry), m_hasTessellation(hasTessellation), m_name(name), m_filePath(filePath) {
		m_shaderProgram = Load();
		if (!m_shaderProgram) LOG_ERROR("[~bShaders~x] ~1%s~x shader failed to compile", name.c_str());
		m_uniformBuffer.RegisterUniforms(m_shaderProgram);
	}

	~Shader() {
		delete m_shaderProgram;
	}

	friend class ShaderManager;
public:

	const ShaderUniformBuffer* GetUniformBuffer() const { return &m_uniformBuffer; }
	const ShaderProperties* GetShaderProperties() const { return &m_properties; }

	const String& GetName() { return m_name; }

	template<typename T>
	void Set(const String_t location, const T* value, uint count) {
		m_uniformBuffer.Set(location, value, count);
	}

	template<typename T>
	void Set(const String_t location, const T& value) {
		m_uniformBuffer.Set(location, value);
	}

	template <>
	void Set<bool>(const String_t location, const bool& value) {
		m_uniformBuffer.Set(location, (int)value);
	}

	void Reload() {
		ShaderProgram* program = Load();
		if (program) {
			delete m_shaderProgram;
			m_shaderProgram = program;
			m_uniformBuffer.Reload(m_shaderProgram);
		};
	}

	void Bind() {
		m_shaderProgram->Bind();
	}

	void Unbind() {
		m_shaderProgram->Unbind();
	}

	void OnImGUI();

	static const String GetAssetTypeName() { return "Shader"; }
};