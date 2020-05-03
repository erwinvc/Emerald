#pragma once

struct UBOBool {
	int val;
	UBOBool() : val(false) {}
	UBOBool(bool b) : val(b) {}
};

struct UBOFloat {
	float val;
	UBOFloat() : val(0.0f) {}
	UBOFloat(float f) : val(f) {}
};


struct UBOVec2 {
	glm::vec2 vec;
	float _padding1;
	float _padding2;

	UBOVec2() : vec(glm::vec2(0.0f, 0.0f)) {}
	UBOVec2(glm::vec2 v) : vec(v) {}
};

struct UBOVec3 {
	glm::vec3 vec;
	float _padding2;

	UBOVec3() : vec(glm::vec3(0.0f, 0.0f, 0.0f)) {}
	UBOVec3(glm::vec3 v) : vec(v) {}
};

struct UBOVec4 {
	glm::vec4 vec;

	UBOVec4() : vec(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)) {}
	UBOVec4(glm::vec3 v) : vec(glm::vec4(v, 0.0f)) {}
	UBOVec4(glm::vec4 v) : vec(v) {}
};

struct UBOMat3 {
	UBOVec4 col1;
	UBOVec4 col2;
	UBOVec4 col3;

	UBOMat3() : col1(UBOVec4()), col2(UBOVec4()), col3(UBOVec4()) {}
	UBOMat3(glm::mat3 m) : col1(glm::column(m, 0)), col2(glm::column(m, 1)), col3(glm::column(m, 2)) {}
};

struct UBOMat4 {
	UBOVec4 col1;
	UBOVec4 col2;
	UBOVec4 col3;
	UBOVec4 col4;

	UBOMat4() : col1(UBOVec4()), col2(UBOVec4()), col3(UBOVec4()), col4(UBOVec4()) {}
	UBOMat4(glm::mat4 m) : col1(glm::column(m, 0)), col2(glm::column(m, 1)), col3(glm::column(m, 2)), col4(glm::column(m, 3)) {}
};

struct GlobalUniforms {
	UBOVec3 _CameraPosition;
	UBOMat4 _Projection;
	UBOMat4 _View;
	UBOMat4 _InverseProjection;
	UBOMat4 _InverseView;
	UBOFloat _BloomFactor;
	UBOBool _SSAOEnabled;
	UBOVec2 _CameraPlanes;
	UBOVec2 _ViewPort;
};

template<typename T>
class UniformBuffer {
private:
	uint m_bufferID;
	uint m_uniformBlockIndex;
public:
	T data;

	UniformBuffer(const Shader* shader) {
		GL(glGenBuffers(1, &m_bufferID));
		GL(glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID));
		GL(glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &data, GL_DYNAMIC_DRAW));

		glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_bufferID);
	}


	~UniformBuffer() {
		GL(glDeleteBuffers(1, &m_bufferID));
	}

	void SetData() {
		GL(glBindBuffer(GL_UNIFORM_BUFFER, m_bufferID));
		GL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data));
		GL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
	}
};