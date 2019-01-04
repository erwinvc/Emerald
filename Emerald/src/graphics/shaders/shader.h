#pragma once

class Shader {
	GLuint m_shaderID;

	String m_name;
	String m_vertex;
	String m_fragment;

	map<String_t, GLuint> m_uniforms;

	GLuint LoadShader(String path, GLuint type) {
		GLuint shader = glCreateShader(type);
		String source = Utils::ReadFile(path);
		if (source.empty()) {
			LOG_ERROR("[~bShaders~x] Failed to load %s shader %s", m_name, (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
			return -1;
		}
		String_t sourceCC = source.c_str();
		glShaderSource(shader, 1, &sourceCC, NULL);
		glCompileShader(shader);

		GLint result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			GLint length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			vector<char> error(length);
			glGetShaderInfoLog(shader, length, &length, &error[0]);
			LOG_ERROR("%s", &error[0]);
			LOG_ERROR("[~bShaders~x] Failed to compile %s shader %s", m_name, (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
			glDeleteShader(shader);
			return -1;
		}
		LOG_PRINT("[~bShaders~x] Compiled ~1%s~x %s", m_name, (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
		return shader;
	}
	GLuint Load() {
		GLuint program = glCreateProgram();
		GLuint vertex = LoadShader(m_vertex, GL_VERTEX_SHADER);
		GLuint fragment = LoadShader(m_fragment, GL_FRAGMENT_SHADER);

		if (vertex == -1 || fragment == -1) {
			glDeleteProgram(program);
			return -1;
		}

		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		return program;
	}
public:
	Shader() {}
	Shader(String name, String vertex, String fragment) : m_name(name), m_vertex(vertex), m_fragment(fragment) {
		m_shaderID = Load();
	}
	virtual ~Shader() { glDeleteShader(m_shaderID); }

	void Set(String_t location, int value) { glUniform1i(m_uniforms[location], value); }
	void Set(String_t location, float value) { glUniform1f(m_uniforms[location], value); }
	void Set(String_t location, float x, float y) { glUniform2f(m_uniforms[location], x, y); }
	void Set(String_t location, Color color) { glUniform4f(m_uniforms[location], color.R, color.G, color.B, color.A); }
	void Set(String_t location, Matrix4 matrix) { glUniformMatrix4fv(m_uniforms[location], 1, GL_TRUE, matrix.elements); }
	void Set(String_t location, Vector4 vector) { glUniform4f(m_uniforms[location], vector.x, vector.y, vector.z, vector.w); }
	void Set(String_t location, Vector3 vector) { glUniform3f(m_uniforms[location], vector.x, vector.y, vector.z); }
	void Set(String_t location, Vector2 vector) { glUniform2f(m_uniforms[location], vector.x, vector.y); }
	void Set(String_t location, boolean value) { Set(location, value ? 1.0f : 0.0f); }

	virtual void Start() {
		glUseProgram(m_shaderID);
	}

	virtual void Stop() {
		glUseProgram(0);
	}

protected:
	void AddUniform(String_t uniform) {
		int uniformLocation = glGetUniformLocation(m_shaderID, uniform);
		if (uniformLocation == 0xffffffff) LOG("[~bShaders~x] Failed to find uniform~1 %s~x in the ~1%s ~xshader", uniform, m_name);
		m_uniforms.emplace(uniform, uniformLocation);
	}
};