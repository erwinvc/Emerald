#pragma once

namespace MeshGenerator {

    /*Create a sphere with x amount of stacks and slices*/
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

        VertexArray* sphereVAO = new VertexArray();
        sphereVAO->AddBuffer(new Buffer(positions.data(), positions.size(), 3), 0, false);

        return new Mesh(sphereVAO, new IndexBuffer(indices.data(), indices.size()));
    }

    /*Create a simple quad*/
   static Mesh* Quad() {
        uint indices[] = { 0, 1, 2, 0, 2, 3 };
        GLfloat vertices[] = { -1, -1, 0,
                               -1,  1, 0,
                                1,  1, 0,
                                1, -1, 0 };

        VertexArray* m_vao = new VertexArray();
        m_vao->AddBuffer(new Buffer(vertices, NUMOF(vertices), 3), 0, false);

        return new Mesh(m_vao, new IndexBuffer(indices, NUMOF(indices)));
    }
}