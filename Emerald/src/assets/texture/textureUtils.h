//#pragma once
//
//namespace TextureUtils
//{
//    void RenderTextureToScreen(uint texture, Vector2 position, Vector2 size)
//    {
//        GLuint quad_VertexArrayID;
//        glGenVertexArrays(1, &quad_VertexArrayID);
//        glBindVertexArray(quad_VertexArrayID);
//
//        static const GLfloat g_quad_vertex_buffer_data[] = {
//            -1.0f, -1.0f, 0.0f,
//            1.0f, -1.0f, 0.0f,
//            -1.0f,  1.0f, 0.0f,
//            -1.0f,  1.0f, 0.0f,
//            1.0f, -1.0f, 0.0f,
//            1.0f,  1.0f, 0.0f,
//        };
//
//        GLuint quad_vertexbuffer;
//        glGenBuffers(1, &quad_vertexbuffer);
//        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
//
//        // Create and compile our GLSL program from the shaders
//        GLuint quad_programID = LoadShaders("Passthrough.vertexshader", "SimpleTexture.fragmentshader");
//        GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");
//        GLuint timeID = glGetUniformLocation(quad_programID, "time");
//    }
//}