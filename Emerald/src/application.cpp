#include "stdafx.h"

static void ErrorCallback(int error, const char* description) {
	LOG_ERROR("[GLFW] %s", description);
}

Application::Application() {
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit()) {
		LOG_ERROR("[GLFW] GLFW failed to initialize");
		return;
	}

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	m_window = new Window("Test", 1920, 1080);

	m_window->MakeContextCurrent();
	m_window->Show();
	m_window->ClearColor(Color(0.5f, 0.7f, 1.0f, 1.0f));

	if (glewInit() != GLEW_OK) {
		LOG_ERROR("GLEW failed to initialize");
		return;
	}

}

Application::~Application() {
}

//void Application::Run() {
//
//	GLuint vbo, vao;
//	glGenVertexArrays(1, &vao);
//	glBindVertexArray(vao);
//
//	GLfloat vertices[] = {
//		0, 0, 0,
//		0, 3, 0,
//		8, 3, 0,
//		8, 0, 0
//	};
//
//	GLfloat colors[144 * 4 * 100];
//	for (int i = 0; i < 144 * 100; i++) {
//		colors[i * 4] = Math::randomf(0.0, 1.0);
//		colors[i * 4 + 1] = Math::randomf(0.0, 1.0);
//		colors[i * 4 + 2] = Math::randomf(0.0, 1.0);
//		colors[i * 4 + 3] = 1.0f;
//	}
//
//	int offset = 0;
//	GLfloat offsetsX[144 * 100];
//	GLfloat offsetsY[144 * 100];
//	for (int y = 0; y < 16 * 10; y++) {
//		for (int x = 0; x < 9 * 10; x++) {
//			offsetsX[offset] = (float)y / 10;
//			offsetsY[offset++] = (float)x / 10;
//		}
//	}
//
//	glGenBuffers(1, &vbo);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(0);
//
//	GLuint offsetsBufferID;
//	glGenBuffers(1, &offsetsBufferID);
//	glBindBuffer(GL_ARRAY_BUFFER, offsetsBufferID);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(offsetsX), offsetsX, GL_STATIC_DRAW);
//	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), 0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribDivisor(1, 1);
//
//	GLuint offsets2BufferID;
//	glGenBuffers(1, &offsets2BufferID);
//	glBindBuffer(GL_ARRAY_BUFFER, offsets2BufferID);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(offsetsY), offsetsY, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), 0);
//	glVertexAttribDivisor(2, 1);
//
//	GLuint colorsBufferID;
//	glGenBuffers(1, &colorsBufferID);
//	glBindBuffer(GL_ARRAY_BUFFER, colorsBufferID);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(3);
//	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
//	glVertexAttribDivisor(3, 1);
//
//	GLushort indices[] = { 0, 1, 2, 2, 3, 0 };
//	GLuint indexArrayBufferID;
//	glGenBuffers(1, &indexArrayBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArrayBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//	float numIndices = 6;
//
//	BasicShader shader;
//	shader.Start();
//	shader.Set("color", Color(0.0f, 0.3f, 0.3f, 1.0f));
//	Matrix4 projection = Matrix4::Orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f);
//	shader.Set("projection", projection);
//
//	while (!m_window->ShouldClose()) {
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		//glDrawArrays(GL_TRIANGLES, 0, 6);
//		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, 144 * 100);
//
//		m_window->SwapBuffers();
//		m_window->PollEvents();
//	}
//}

void Application::Cleanup() {
	delete m_window;
}

void Application::Run() {

	GLfloat vertices[] = {
		0, 0, 0,
		0, 3, 0,
		8, 3, 0,
		8, 0, 0
	};

	GLfloat colors[144 * 4 * 100];
	for (int i = 0; i < 144 * 100; i++) {
		colors[i * 4] = Math::randomf(0.0, 1.0);
		colors[i * 4 + 1] = Math::randomf(0.0, 1.0);
		colors[i * 4 + 2] = Math::randomf(0.0, 1.0);
		colors[i * 4 + 3] = 1.0f;
	}

	int offset = 0;
	GLfloat offsetsX[144 * 100];
	GLfloat offsetsY[144 * 100];
	GLfloat offsetsZ[144 * 100];
	for (int y = 0; y < 16 * 10; y++) {
		for (int x = 0; x < 9 * 10; x++) {
			offsetsX[offset] = ((float)(y - (16 * 10 / 2)) / 10);
			offsetsY[offset] = ((float)(x - (9 * 10 / 2)) / 10);
			offsetsZ[offset++] = Math::randomf(-50, 50);
		}
	}

	GLushort indices[] = { 0, 1, 2, 2, 3, 0 };


	VertexArray vao;
	vao.AddBuffer(new Buffer(vertices, NUMOF(vertices), 3), 0, false);
	vao.AddBuffer(new Buffer(offsetsX, NUMOF(offsetsX), 1), 1, true);
	vao.AddBuffer(new Buffer(offsetsY, NUMOF(offsetsY), 1), 2, true);
	vao.AddBuffer(new Buffer(offsetsZ, NUMOF(offsetsZ), 1), 3, true);
	vao.AddBuffer(new Buffer(colors, NUMOF(colors), 4), 4, true);
	IndexBuffer ibo(indices, 6);

	
	Matrix4 projection = Matrix4::Perspective(65.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

	BasicShader shader;
	shader.Start();
	shader.Set("color", Color(0.0f, 0.3f, 0.3f, 1.0f));
	shader.Set("projection", projection);


	float sinus = 0;
	while (!m_window->ShouldClose()) {
		//Sleep(1);
		sinus += 0.01f;
		LOG("%f",Math::sin(sinus));
		float FOV = ((Math::sin(sinus) + 1) / 2) * 150 + 20; 
		Matrix4 projection = Matrix4::Perspective(FOV, 16.0f / 9.0f, 0.1f, 1000.0f);
		shader.Set("projection", projection);

		glClear(GL_COLOR_BUFFER_BIT);

		vao.Bind();
		ibo.Bind();
		glDrawElementsInstanced(GL_TRIANGLES, ibo.GetCount(), GL_UNSIGNED_SHORT, 0, 144 * 100);
		vao.Unbind();
		ibo.Unbind();

		m_window->SwapBuffers();
		m_window->PollEvents();
	}
}