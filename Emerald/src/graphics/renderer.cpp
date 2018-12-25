#include "stdafx.h"
#include "renderer.h"


Renderer::Renderer() : m_buffer(nullptr) {
	Initialize();
}


Renderer::~Renderer() {
	delete[] m_buffer;
	delete m_buffer;
}

void Renderer::Initialize() {
	const uint MAX_OBJECTS = 100;

	//m_buffer = new VertexArray(QUAD_VERTEX_BUFFER_COUNT * qaudLayout.GetStride());

}


void Renderer::Submit(Entity& entity) {

}
void Renderer::Flush() {

}