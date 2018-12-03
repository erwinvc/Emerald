#pragma once

class Shader {
	GLuint m_shaderID;

	String m_name;
	String m_vertex;
	String m_fragment;

public:
	Shader() {}
	Shader(String name, String vertex, String fragment) : m_name(name), m_vertex(vertex), m_fragment(fragment) {
		m_shaderID = Load();
	}

	~Shader() { glDeleteShader(m_shaderID); }

	GLuint LoadShader(String path, GLuint type) {
		GLuint shader = glCreateShader(type);
		String source = ReadFile(path);
		if (source.empty()) {
			LOG_ERROR("[Shader] Failed to load %s shader %s", m_name, (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
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
			LOG_ERROR("[Shader] Failed to compile %s shader %s", m_name, (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
			glDeleteShader(shader);
			return -1;
		}
		LOG_PRINT("[Shader] Compiled %s %s", m_name, (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
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

	void Start() {
		glUseProgram(m_shaderID);
	}

	void Stop() {
		glUseProgram(0);
	}
};