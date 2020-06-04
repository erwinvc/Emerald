#include "stdafx.h"

uint Shader::s_boundShader = 0xffffffff;

Shader::Shader(const String& name, const Path& filePath, bool hasGeometry, bool hasTessellation) : m_shaderProgram(nullptr), m_hasGeometry(hasGeometry), m_hasTessellation(hasTessellation), m_name(name), m_filePath(filePath) {
	m_shaderProgram = Load();
	if (!m_shaderProgram) LOG_ERROR("[~bShaders~x] ~1%s~x shader failed to compile", name.c_str());
	m_uniformBuffer.RegisterUniforms(m_shaderProgram);
}

Shader::~Shader() {
	DELETE(m_shaderProgram);
}

GLuint Shader::LoadShader(const String& path, GLuint type) {
	GL(uint shader = glCreateShader(type));
	if (!FileSystem::DoesFileExist(path)) LOG_WARN("[~bShaders~x] ~1%s ~xshader ~1%s ~xat ~1%s does not exist", m_name.c_str(), GLUtils::ShaderTypeToString(type), path.c_str());

	String source = "";
	String logOutput = "";
	if (!Shadinclude::load(path, source, logOutput)) {
		LOG_WARN("[~bShaders~x] Failed to load ~1%s ~xshader ~1%s ~xat ~1%s~x [~1%s]", m_name.c_str(), GLUtils::ShaderTypeToString(type), path.c_str(), logOutput.c_str());
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

bool Shader::AddShaderToProgram(ShaderProgram* program, const String& shader, GLuint shaderType) {
	GLuint shaderHandle = LoadShader(shader, shaderType);
	if (shaderHandle == 0xffffffff) return true;
	program->AttachShader(shaderHandle);
	return false;
}

ShaderProgram* Shader::Load() {
	m_properties.Clear();
	ShaderProgram* shaderProgram = NEW(ShaderProgram());
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
		DELETE(shaderProgram);
		shaderProgram = nullptr;
	} else {
		shaderProgram->LinkAndValidate();
		shaderProgram->DeleteAttachedShaders();
	}

	return shaderProgram;
}

void Shader::Reload() {
	ShaderProgram* program = Load();
	if (program) {
		delete m_shaderProgram;
		m_shaderProgram = program;
		m_uniformBuffer.Reload(m_shaderProgram);
	};
}

void Shader::Bind() {
	if (s_boundShader == m_shaderProgram->GetHandle())return;
	m_shaderProgram->Bind();
	s_boundShader = m_shaderProgram->GetHandle();
}

void Shader::Unbind() {
	m_shaderProgram->Unbind();
	s_boundShader = 0xffffffff;
}

void Shader::OnImGUI() {
	Bind();
	if (ImGui::Button(Format_t("Reload##%s", m_name.c_str()))) {
		GetClient()->QueueTask([this] {Reload(); });
	}
	m_uniformBuffer.OnImGUI();
}