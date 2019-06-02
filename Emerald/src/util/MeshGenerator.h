#pragma once

namespace MeshGenerator {

    /*Create a sphere with x amount of stacks and slices*/
   static Mesh* Sphere(int stacks = 20, int slices = 20) {

	   vector<GLfloat> positions;
	   vector<uint> indices;

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

		shared_ptr<VertexArray> vao(new VertexArray());
		shared_ptr<IndexBuffer> ibo(new IndexBuffer(indices.data(), indices.size()));
		vao->AddBuffer(NEW(Buffer(positions.data(), positions.size(), 3)), 0, false);

        return NEW(Mesh(vao, ibo));
    }

    /*Create a simple quad*/
   static Mesh* Quad() {
        uint indices[] = { 0, 1, 2, 0, 2, 3 };
        GLfloat vertices[] = { -1, -1, 0,
                               -1,  1, 0,
                                1,  1, 0,
                                1, -1, 0 };

		shared_ptr<VertexArray> vao(new VertexArray());
		shared_ptr<IndexBuffer> ibo(new IndexBuffer(indices, NUMOF(indices)));
		vao->AddBuffer(NEW(Buffer(vertices, NUMOF(vertices), 3)), 0, false);

		return NEW(Mesh(vao, ibo));
    }
}