#pragma once
#include <vector>

namespace emerald {
	namespace MeshGenerator {
		/*Create a sphere with x amount of stacks and slices*/
		/*
		static Mesh* Sphere(int stacks = 20, int slices = 20) {

			std::vector<GLfloat> positions;
			std::vector<uint> indices;

			for (int i = 0; i <= stacks; ++i) {
				float V = (float)i / (float)stacks;
				float phi = V * Math::PI;

				for (int j = 0; j <= slices; ++j) {
					float U = (float)j / (float)slices;
					float theta = U * (Math::PI * 2);

					float x = Math::Cos(theta) * Math::Sin(phi);
					float y = Math::Cos(phi);
					float z = Math::Sin(theta) * Math::Sin(phi);

					positions.push_back(x);
					positions.push_back(y);
					positions.push_back(z);
				}
			}

			for (int i = 0; i < slices * stacks + slices; ++i) {
				indices.push_back(GLuint(i));
				indices.push_back(GLuint(i + slices + 1));
				indices.push_back(GLuint(i + slices));

				indices.push_back(GLuint(i + slices + 1));
				indices.push_back(GLuint(i));
				indices.push_back(GLuint(i + 1));
			}

			ManagedRef<VertexArray> vao(new VertexArray());
			ManagedRef<IndexBuffer> ibo(new IndexBuffer(indices.data(), indices.size()));

			BufferLayout layout = {
				{ShaderDataType::Float3, "position", 0} };
			vao->AddBuffer(NEW(VertexBuffer(positions.data(), positions.size() / 3, layout)));
			vao->ApplyLayouts();
			return NEW(Mesh(vao, ibo));
		}
		*/
		/*Create a simple quad*/
		/*
		static Mesh* Quad() {
			struct Vertex {
				Vector3 m_position;
				Vector2 m_uv;

				Vertex(Vector3 pos, Vector2 uv) : m_position(pos), m_uv(uv) {}
			};

			Vertex vertices[4] = {
			Vertex(Vector3(-1, -1, 0), Vector2(0, 0)),
			Vertex(Vector3(-1,  1, 0), Vector2(0, 1)),
			Vertex(Vector3(1,  1, 0),  Vector2(1, 1)),
			Vertex(Vector3(1, -1, 0),  Vector2(1, 0))
			};

			uint indices[] = { 0, 1, 2, 0, 2, 3 };


			ManagedRef<VertexArray> vao(new VertexArray());
			ManagedRef<IndexBuffer> ibo(new IndexBuffer(indices, NUMOF(indices)));

			BufferLayout layout = {
				{ShaderDataType::Float3, "position", 0},
				{ShaderDataType::Float2, "uv", 1}
			};
			vao->AddBuffer(NEW(VertexBuffer(vertices, NUMOF(vertices), layout)));
			vao->ApplyLayouts();
			return NEW(Mesh(vao, ibo));
		}
		*/
	}
}