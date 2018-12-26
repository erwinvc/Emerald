#include "stdafx.h"


#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include <assimp/Importer.hpp>

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

vector<Entity> entities;
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

    uint indices[] = { 0, 1, 2, 2, 3, 0 };


    VertexArray vao;
    vao.AddBuffer(new Buffer(vertices, NUMOF(vertices), 3), 0, false);
    vao.AddBuffer(new Buffer(offsetsX, NUMOF(offsetsX), 1), 1, true);
    vao.AddBuffer(new Buffer(offsetsY, NUMOF(offsetsY), 1), 2, true);
    vao.AddBuffer(new Buffer(offsetsZ, NUMOF(offsetsZ), 1), 3, true);
    vao.AddBuffer(new Buffer(colors, NUMOF(colors), 4), 4, true);
    IndexBuffer ibo(indices, 6);

    //for (uint i = 0; i < scene->mRootNode->mNumVertices; i++) {
    //	MeshVertex vertex;
    //
    //	vertex.x = mesh->mVertices[i].x;
    //	vertex.y = mesh->mVertices[i].y;
    //	vertex.z = mesh->mVertices[i].z;
    //
    //	vertex.normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
    //
    //
    //
    //	if (mesh->mTextureCoords[0]) {
    //		vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
    //		vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
    //	}
    //
    //	vertices.push_back(vertex);
    //}
    //
    //for (uint i = 0; i < mesh->mNumFaces; i++) {
    //	aiFace face = mesh->mFaces[i];
    //
    //	for (uint j = 0; j < face.mNumIndices; j++)
    //		indices.push_back(face.mIndices[j]);
    //}


    //std::vector<tinyobj::material_t> materials;
    //tinyobj::attrib_t attrib;
    //std::vector<tinyobj::shape_t> shapes;
    //std::string warn;
    //std::string err;
    //LOG("%s", tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "Adv4Greenhouse.obj", "", true) ? "Yes" : "No");


    //OBJ_LOADER
    //objl::Loader loader;
    //LOG("%s", loader.LoadFile("dragon.obj") ? "Yes" : "No");

    //LOG("Vertices: %d", loader.LoadedVertices.size());
    //LOG("Name: %s", loader.LoadedMeshes[0].MeshName.c_str());

    GLfloat cubeVertices[] = {
                -0.5f,0.5f,-0.5f,
                -0.5f,-0.5f,-0.5f,
                0.5f,-0.5f,-0.5f,
                0.5f,0.5f,-0.5f,

                -0.5f,0.5f,0.5f,
                -0.5f,-0.5f,0.5f,
                0.5f,-0.5f,0.5f,
                0.5f,0.5f,0.5f,

                0.5f,0.5f,-0.5f,
                0.5f,-0.5f,-0.5f,
                0.5f,-0.5f,0.5f,
                0.5f,0.5f,0.5f,

                -0.5f,0.5f,-0.5f,
                -0.5f,-0.5f,-0.5f,
                -0.5f,-0.5f,0.5f,
                -0.5f,0.5f,0.5f,

                -0.5f,0.5f,0.5f,
                -0.5f,0.5f,-0.5f,
                0.5f,0.5f,-0.5f,
                0.5f,0.5f,0.5f,

                -0.5f,-0.5f,0.5f,
                -0.5f,-0.5f,-0.5f,
                0.5f,-0.5f,-0.5f,
                0.5f,-0.5f,0.5f
    };

    uint cubeIndices[] = {
                0,1,3,
                3,1,2,
                4,5,7,
                7,5,6,
                8,9,11,
                11,9,10,
                12,13,15,
                15,13,14,
                16,17,19,
                19,17,18,
                20,21,23,
                23,21,22
    };


    //vao2->AddBuffer(new Buffer(colors, NUMOF(colors), 4), 1, false);

    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile("res/dragon.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);
        aiNode* rnode = scene->mRootNode;
        for (int k = 0; k < rnode->mNumChildren; k++) {
            aiNode* node = rnode->mChildren[k];
            for (uint i = 0; i < node->mNumMeshes; i++) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

                vector<GLfloat> texcoords;
                for (int i = 0; i < mesh[0].mNumVertices; i++) {
                    texcoords.push_back(mesh[0].mTextureCoords[0][i].x);
                    texcoords.push_back(mesh[0].mTextureCoords[0][i].y);
                }

                //Model
                VertexArray* vaoModel = new VertexArray();
                vaoModel->AddBuffer(new Buffer((float*)mesh->mVertices, mesh->mNumVertices * 3, 3), 0, false);
                vaoModel->AddBuffer(new Buffer(&texcoords[0], texcoords.size(), 2), 1, false);
                vaoModel->AddBuffer(new Buffer((float*)mesh->mNormals, mesh->mNumVertices * 3, 3), 2, false);
                vaoModel->AddBuffer(new Buffer((float*)mesh->mTangents, mesh->mNumVertices * 3, 3), 3, false);

                LOG("%s %s",mesh->HasTangentsAndBitangents() ? "yes" : "no", mesh->mName.C_Str());

                vector<uint> shorts;
                for (uint i = 0; i < mesh->mNumFaces; i++) {
                    aiFace face = mesh->mFaces[i];

                    for (uint j = 0; j < face.mNumIndices; j++)
                        shorts.push_back(face.mIndices[j]);
                }
                IndexBuffer* iboModel = new IndexBuffer((&shorts[0]), shorts.size());

                Mesh meshh(vaoModel, iboModel);
                Entity ent(meshh);
                entities.push_back(ent);
            }
        }
        importer.FreeScene();
        //aiReleaseImport(scene);
    }

    //Cube
    VertexArray* vao2 = new VertexArray();
    vao2->AddBuffer(new Buffer(cubeVertices, NUMOF(cubeVertices), 3), 0, false);
    IndexBuffer* ibo2 = new IndexBuffer(cubeIndices, NUMOF(cubeIndices));

    Mesh mesh(vao2, ibo2);
    Entity entity(mesh);

    //Camera camera;
    BasicShader* shader = new BasicShader();
    shader->Start();

    Texture tex("res/white.png");
    Texture tex2("res/image.png");
    Texture tex3("res/rock.jpg");
    Texture texNormal("res/normal.png");
    Texture rockNormal("res/normalNew.jpg");

    tex.Bind(0);
    tex2.Bind(1);
    tex3.Bind(2);
    texNormal.Bind(3);
    rockNormal.Bind(4);

    Matrix4 projection = Matrix4::Perspective(65.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    shader->Set("projectionMatrix", projection);


    Matrix4 cameraa = Matrix4::LookAt(Vector3(0, 0, 0.1f), Vector3(0, 0, 0), Vector3(0, 1, 0));
    shader->Set("viewMatrix", cameraa);

    float sinus = 0;
    float sinus2 = 0;

    shader->Set("eyePos", cameraa.GetColumn(3));
    shader->Set("intensity", 0.3f);
    shader->Set("lightColor", Color(0.6f, 0.5f, 0.5f, 1));
    ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup style
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window->GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    float scale = 1;
    float x = 1.1f;
    float y = -3.7f;
    float z = -6.0f;
    float sepx = 0;
    float sepy = 0;
    float sepz = 0;
    float rotx = 0;
    float roty = 90;
    float rotz = 0;
    int off = 0;
    int texture = 0;
    int normal = 3;
    Color color;
    Vector3 position = Vector3(Math::HALF_PI);
    float specIntensity = 1;
    float specPower = 1;
    while (!m_window->ShouldClose()) {
        //Sleep(1);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader->Start();
        sinus += 0.001f;
        sinus2 += 0.003f;
        float FOV = ((Math::sin(sinus) + 1) / 2) * 150 + 20;
        //entity.m_position.x = Math::sin(sinus);
        entity.m_rotation.x = sinus * 20;
        entity.m_rotation.y = Math::sin(sinus) * 20;
        //shader.Set("viewMatrix", camera.GetViewMatrix());
        //Matrix4 projection = Matrix4::Perspective(FOV, 16.0f / 9.0f, 0.1f, 1000.0f);
        //shader->Set("projectionMatrix", projection);
        entity.m_rotation.x = sinus * 180;
        entity.m_rotation.y = sinus * 90;
        entity.m_rotation.z = sinus * 360;

        shader->Set("direction", position);
        shader->Set("specularIntensity", specIntensity);
        shader->Set("specularPower", specPower);

        shader->Set("tex", texture);
        shader->Set("texNormal", normal);
        shader->Set("color", color);
        const float s = 0.00006f;
        float f = -(entities.size() * s / 2);
        for (auto& e : entities) {
            e.m_position.x = x + sepx * f;
            e.m_position.y = y + sepy * f;
            e.m_position.z = z + sepz * f;
            e.m_rotation.x = rotx;
            e.m_rotation.y = roty;
            e.m_rotation.z = rotz;
            e.m_scale = scale;
            //e.m_position.x += Math::cos(sinus2) * f;
            //e.m_position.y += Math::sin(sinus2) * f;
            f += s;
            //e.m_position.z = -2;
            //e.m_rotation = entity.m_rotation;
            e.Draw(*shader);
        }

        //entities[off].Draw(*shader);
        //model.m_position.z = -2;
        //model.m_rotation = entity.m_rotation;

        //entity.Draw(*shader);
        //model.Draw(*shader);
        shader->Stop();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Hello, world!");
        ImGui::DragFloat("Scale", &scale, 0.001f);
        ImGui::DragFloat("X", &x, 0.1f);
        ImGui::DragFloat("Y", &y, 0.1f);
        ImGui::DragFloat("Z", &z, 0.1f);
        ImGui::DragFloat("RotX", &rotx, 0.1f);
        ImGui::DragFloat("RotY", &roty, 0.1f);
        ImGui::DragFloat("RotZ", &rotz, 0.1f);

        ImGui::DragFloat3("Position", (float*)&position, 0.01f);

        ImGui::DragFloat("SepX", &sepx, 1);
        ImGui::DragFloat("SepY", &sepy, 1);
        ImGui::DragFloat("SepZ", &sepz, 1);
        ImGui::SliderInt("Offset", &off, 0, entities.size() - 1);
        ImGui::SliderInt("Texture", &texture, 0, 2);
        ImGui::SliderInt("Normal", &normal, 3, 4);
        ImGui::SliderFloat("specIntensity", &specIntensity, 0, 1);
        ImGui::SliderFloat("specPower", &specPower, 0, 1);
        ImGui::ColorPicker4("Color", (float*)&color);
        ImGui::End();

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_window->SwapBuffers();
        m_window->PollEvents();
    }
}