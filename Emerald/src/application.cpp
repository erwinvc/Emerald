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

void Application::Run() {

	GLuint vbo, vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		 0.5f, 0.5f, 0.0f,
		 0.5f, 0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	Shader shader("test", "src/graphics/shaders/basic.vert", "src/graphics/shaders/basic.frag");
	shader.Start();

	while (!m_window->ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		m_window->SwapBuffers();
		m_window->PollEvents();
	}
}
void Application::Cleanup() {
	delete m_window;
}