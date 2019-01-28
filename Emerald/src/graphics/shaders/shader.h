#pragma once

class Shader {
    GLuint m_shaderID;

    String m_name;
    String m_vertex;
    String m_fragment;

    map<String, GLuint> m_uniforms;

    GLuint LoadShader(String path, GLuint type) {
        GL(uint shader = glCreateShader(type));
        String source = Utils::ReadFile(path);
        if (source.empty()) {
            LOG_ERROR("[~bShaders~x] Failed to load %s shader %s", m_name, (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
            return -1;
        }
        String_t sourceCC = source.c_str();
        GL(glShaderSource(shader, 1, &sourceCC, 0));
        GL(glCompileShader(shader));

        GLint result;
        GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE) {
            GLint length;
            GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
            vector<char> error(length);
            GL(glGetShaderInfoLog(shader, length, &length, &error[0]));
            LOG_ERROR("%s", &error[0]);
            LOG_ERROR("[~bShaders~x] Failed to compile %s shader %s", m_name, (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
            GL(glDeleteShader(shader));
            return -1;
        }
        LOG_PRINT("[~bShaders~x] Compiled ~1%s~x %s", m_name, (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
        return shader;
    }
    GLuint Load() {
        GL(uint program = glCreateProgram());
        uint vertex = LoadShader(m_vertex, GL_VERTEX_SHADER);
        uint fragment = LoadShader(m_fragment, GL_FRAGMENT_SHADER);

        if (vertex == -1 || fragment == -1) {
            GL(glDeleteProgram(program));
            return -1;
        }

        GL(glAttachShader(program, vertex));
        GL(glAttachShader(program, fragment));
        GL(glLinkProgram(program));
        GL(glValidateProgram(program));

        GL(glDeleteShader(vertex));
        GL(glDeleteShader(fragment));

        return program;
    }
public:
    Shader() {}
    Shader(String name, String vertex, String fragment) : m_name(name), m_vertex(vertex), m_fragment(fragment) {
        m_shaderID = Load();
        if (m_shaderID != -1) RegisterUniforms();
    }
    virtual ~Shader() { GL(glDeleteProgram(m_shaderID)); }

    void Set(const String_t location, const int value) { GL(glUniform1i(m_uniforms[location], value)); }
    void Set(const String_t location, const float value) { GL(glUniform1f(m_uniforms[location], value)); }
    void Set(const String_t location, const float x, const float y) { GL(glUniform2f(m_uniforms[location], x, y)); }
    void Set(const String_t location, const Color& color) { GL(glUniform4f(m_uniforms[location], color.R, color.G, color.B, color.A)); }
    void Set(const String_t location, float x, float y, float z) { GL(glUniform3f(m_uniforms[location], x, y, z)); }
    void Set(const String_t location, const Matrix4& matrix) { GL(glUniformMatrix4fv(m_uniforms[location], 1, GL_TRUE, matrix.elements)); }
    void Set(const String_t location, const Vector4& vector) { GL(glUniform4f(m_uniforms[location], vector.x, vector.y, vector.z, vector.w)); }
    void Set(const String_t location, const Vector3& vector) { GL(glUniform3f(m_uniforms[location], vector.x, vector.y, vector.z)); }
    void Set(const String_t location, const Vector2& vector) { GL(glUniform2f(m_uniforms[location], vector.x, vector.y)); }
    void Set(const String_t location, const boolean value) { Set(location, value ? 1.0f : 0.0f); }

    virtual void Start() {
        GL(glUseProgram(m_shaderID));
    }

    virtual void Stop() {
        GL(glUseProgram(0));
    }

protected:
    void AddUniform(String_t uniform) {
        GL(int uniformLocation = glGetUniformLocation(m_shaderID, uniform));
        if (uniformLocation == 0xffffffff) LOG("[~bShaders~x] Failed to find uniform~1 %s~x in the ~1%s ~xshader", uniform, m_name);
        m_uniforms.emplace(uniform, uniformLocation);
    }
    void RegisterUniforms() {
        GLint count;
        GLint size;
        GLenum type;
        const GLsizei bufSize = 64;
        GLchar name[bufSize];
        GLsizei length;
        GL(glGetProgramiv(m_shaderID, GL_ACTIVE_UNIFORMS, &count));
        for (int i = 0; i < count; i++) {
            GL(glGetActiveUniform(m_shaderID, (GLuint)i, bufSize, &length, &size, &type, name));
            if (size > 1) {
                String temp = name;
                temp = temp.substr(0, temp.find("[", 0));
                for (int j = 0; j < size; j++) AddUniform(va("%s[%d]", temp.c_str(), j).c_str());
            } else AddUniform(name);
        }
    }
};