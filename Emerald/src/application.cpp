#include "stdafx.h"

Application* Application::g_instance = 0;

static void ErrorCallback(int error, const char* description) {
    LOG_ERROR("[GLFW] %s", description);
}

Application::Application() : m_running(true) {
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

    m_window = new Window("Test", 1920, 1080, [this](Event& e) { OnEvent(e); });

    m_window->MakeContextCurrent();
    m_window->Show();
    m_window->ClearColor(Color(0.5f, 0.7f, 1.0f, 1.0f));

    if (glewInit() != GLEW_OK) {
        LOG_ERROR("GLEW failed to initialize");
        return;
    }

    GetTextureManager()->Initialize();
    GetMaterialManager()->Initialize();
}

void Application::OnEvent(Event& e) {
    e.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowClose(); });
    e.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) { glViewport(0, 0, e.GetWidth(), e.GetHeight()); return true; });
}

bool Application::OnWindowClose() {
    m_running = false;
    return true;
}

float scale = 1;
float x = 1.1f;
float y = -3.7f;
float z = -6.0f;
float rotx = 0;
float roty = 90;
float rotz = 0;
int texture = 0;
int normal = 4;
int height = 0;
Color color(1, 1, 1, 1);
Vector2 UVScale(1, 1);
Color background;
Color directional(0.6f, 0.5f, 0.5f, 1);
Vector3 position = Vector3(Math::HALF_PI);
float specIntensity = 1;
float specPower = 1;
float var = 1;
float sinus = 0;
float sinus2 = 0;
FreeCam camera;
Shader* shader;
Entity entity;
vector<Entity> entities;
Renderer* renderer;
Vector3 attenuation(1, 0.01f, 0.002f);
float ambientIntensity = 0.1f;

//PhongShader* phong;

Deferred* deferred;

void Application::Run() {

    //GLfloat vertices[] = {
    //    0, 0, 0,
    //    0, 3, 0,
    //    8, 3, 0,
    //    8, 0, 0
    //};
    //
    //GLfloat colors[144 * 4 * 100];
    //for (int i = 0; i < 144 * 100; i++) {
    //    colors[i * 4] = Math::RandomF(0.0, 1.0);
    //    colors[i * 4 + 1] = Math::RandomF(0.0, 1.0);
    //    colors[i * 4 + 2] = Math::RandomF(0.0, 1.0);
    //    colors[i * 4 + 3] = 1.0f;
    //}
    //
    //int offset = 0;
    //GLfloat offsetsX[144 * 100];
    //GLfloat offsetsY[144 * 100];
    //GLfloat offsetsZ[144 * 100];
    //for (int y = 0; y < 16 * 10; y++) {
    //    for (int x = 0; x < 9 * 10; x++) {
    //        offsetsX[offset] = ((float)(y - (16 * 10 / 2)) / 10);
    //        offsetsY[offset] = ((float)(x - (9 * 10 / 2)) / 10);
    //        offsetsZ[offset++] = Math::RandomF(-50, 50);
    //    }
    //}
    //
    //uint indices[] = { 0, 1, 2, 2, 3, 0 };
    //
    //
    //VertexArray vao;
    //vao.AddBuffer(new Buffer(vertices, NUMOF(vertices), 3), 0, false);
    //vao.AddBuffer(new Buffer(offsetsX, NUMOF(offsetsX), 1), 1, true);
    //vao.AddBuffer(new Buffer(offsetsY, NUMOF(offsetsY), 1), 2, true);
    //vao.AddBuffer(new Buffer(offsetsZ, NUMOF(offsetsZ), 1), 3, true);
    //vao.AddBuffer(new Buffer(colors, NUMOF(colors), 4), 4, true);
    //IndexBuffer ibo(indices, 6);
    //
    //GLfloat cubeVertices[] = {
    //            -0.5f,0.5f,-0.5f,
    //            -0.5f,-0.5f,-0.5f,
    //            0.5f,-0.5f,-0.5f,
    //            0.5f,0.5f,-0.5f,
    //
    //            -0.5f,0.5f,0.5f,
    //            -0.5f,-0.5f,0.5f,
    //            0.5f,-0.5f,0.5f,
    //            0.5f,0.5f,0.5f,
    //
    //            0.5f,0.5f,-0.5f,
    //            0.5f,-0.5f,-0.5f,
    //            0.5f,-0.5f,0.5f,
    //            0.5f,0.5f,0.5f,
    //
    //            -0.5f,0.5f,-0.5f,
    //            -0.5f,-0.5f,-0.5f,
    //            -0.5f,-0.5f,0.5f,
    //            -0.5f,0.5f,0.5f,
    //
    //            -0.5f,0.5f,0.5f,
    //            -0.5f,0.5f,-0.5f,
    //            0.5f,0.5f,-0.5f,
    //            0.5f,0.5f,0.5f,
    //
    //            -0.5f,-0.5f,0.5f,
    //            -0.5f,-0.5f,-0.5f,
    //            0.5f,-0.5f,-0.5f,
    //            0.5f,-0.5f,0.5f
    //};
    //
    //uint cubeIndices[] = {
    //            0,1,3,
    //            3,1,2,
    //            4,5,7,
    //            7,5,6,
    //            8,9,11,
    //            11,9,10,
    //            12,13,15,
    //            15,13,14,
    //            16,17,19,
    //            19,17,18,
    //            20,21,23,
    //            23,21,22
    //};

    Assimp::Importer importer;
    const aiScene* ascene = importer.ReadFile("res/cursor.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);
    aiNode* arnode = ascene->mRootNode;
    aiNode* anode = arnode->mChildren[0];
    aiMesh* amesh = ascene->mMeshes[anode->mMeshes[0]];

    vector<uint> shorts;
    for (uint i = 0; i < amesh->mNumFaces; i++) {
        aiFace face = amesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; j++)
            shorts.push_back(face.mIndices[j]);
    }

    vector<GLfloat> texcoords;
    for (uint i = 0; i < amesh[0].mNumVertices; i++) {
        texcoords.push_back(amesh[0].mTextureCoords[0][i].x);
        texcoords.push_back(amesh[0].mTextureCoords[0][i].y);
    }

    uint num = amesh->mNumVertices;
    int num2 = (int)shorts.size();
    renderer = new Renderer(num, num2, (float*)amesh->mVertices, (float*)amesh->mNormals, texcoords.data(), (float*)amesh->mTangents, shorts.data());

    //{
    //    Assimp::Importer importer;
    //    const aiScene* scene = importer.ReadFile("res/steyerdorf.obj", aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);
    //    aiNode* rnode = scene->mRootNode;
    //    for (uint k = 0; k < rnode->mNumChildren; k++) {
    //        aiNode* node = rnode->mChildren[k];
    //        for (uint i = 0; i < node->mNumMeshes; i++) {
    //            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    //
    //            vector<GLfloat> texcoords;
    //            for (uint i = 0; i < mesh[0].mNumVertices; i++) {
    //                texcoords.push_back(mesh[0].mTextureCoords[0][i].x);
    //                texcoords.push_back(mesh[0].mTextureCoords[0][i].y);
    //            }
    //
    //            //Model
    //            VertexArray* vaoModel = new VertexArray();
    //            vaoModel->AddBuffer(new Buffer((float*)mesh->mVertices, mesh->mNumVertices * 3, 3), 0, false);
    //            vaoModel->AddBuffer(new Buffer(texcoords.data(), texcoords.size(), 2), 1, false);
    //            vaoModel->AddBuffer(new Buffer((float*)mesh->mNormals, mesh->mNumVertices * 3, 3), 2, false);
    //            vaoModel->AddBuffer(new Buffer((float*)mesh->mTangents, mesh->mNumVertices * 3, 3), 3, false);
    //
    //            LOG("%s %s", mesh->HasTangentsAndBitangents() ? "yes" : "no", mesh->mName.C_Str());
    //
    //            vector<uint> shorts;
    //            for (uint i = 0; i < mesh->mNumFaces; i++) {
    //                aiFace face = mesh->mFaces[i];
    //
    //                for (uint j = 0; j < face.mNumIndices; j++)
    //                    shorts.push_back(face.mIndices[j]);
    //            }
    //            IndexBuffer* iboModel = new IndexBuffer(shorts.data(), shorts.size());
    //
    //            Mesh meshh(vaoModel, iboModel);
    //            Entity ent(meshh);
    //            entities.push_back(ent);
    //        }
    //    }
    //    importer.FreeScene();
    //}

    deferred = new Deferred();
    deferred->Initialize(m_window, camera);

    //Cube
    //VertexArray* vao2 = new VertexArray();
    //vao2->AddBuffer(new Buffer(cubeVertices, NUMOF(cubeVertices), 3), 0, false);
    //IndexBuffer* ibo2 = new IndexBuffer(cubeIndices, NUMOF(cubeIndices));

    //Mesh mesh(vao2, ibo2);
    //entity = Entity(mesh);

    //shader = new Shader("Basic", "src/graphics/shaders/basic.vert", "src/graphics/shaders/basic.frag");
    //shader->Start();
    //
    //Texture tex("res/white.png");
    //Texture tex2("res/image.png");
    //Texture tex3("res/rock.jpg");
    //Texture obsidian("res/obsidian.png");
    //Texture texNormal("res/normal.png");
    //Texture rockNormal("res/normalNew.jpg");
    //Texture obsidianNormal("res/obsidianNormal.png");
    //Texture testNormal("res/testNormal.png");
    //Texture heightMap("res/heightMap.png");
    //
    //
    //tex.Bind(0);
    //tex2.Bind(1);
    //tex3.Bind(2);
    //obsidian.Bind(3);
    //texNormal.Bind(4);
    //rockNormal.Bind(5);
    //obsidianNormal.Bind(6);
    //testNormal.Bind(7);
    //heightMap.Bind(8);
    //
    //Matrix4 projection = Matrix4::Perspective(65.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    //shader->Set("projectionMatrix", projection);
    //
    //shader->Set("intensity", 0.3f);

    //PHONG
    //phong = new PhongShader();
    //phong->Start();
    //
    //phong->Set("texDiffuse", 1);
    //phong->Set("texNormal", 7);
    //
    //phong->Set("projectionMatrix", projection);

    GetKeyboard()->Initialize(m_window);
    GetMouse()->Initialize(m_window);
    GetImGuiManager()->Initialize(m_window);

    glEnable(GL_DEPTH_TEST);

    /*Main loop*/
    m_timer = new Timer();
    m_timeStep = new TimeStep(m_timer->Get());
    long timer = m_timer->Get();
    float updateTimer = m_timer->Get();
    float updateTick = 1000.0f / 60.0f;
    double delta = 0;
    int frames = 0, updates = 0;
    while (m_running) {
        m_window->Clear();
        float now = m_timer->Get();
        if (now - updateTimer > updateTick) {
            m_timeStep->Update(now);
            Update(*m_timeStep);
            updates++;
            updateTimer += updateTick;
        }
        delta += (now - updateTimer) / updateTick;
        //Update(delta);
        //while (delta >= 1.0) {
        //    FixedUpdate();
        //    updates++;
        //    delta--;
        //}
        Render();
        frames++;
        if (glfwGetTime() - timer > 1.0) {
            m_window->SetTitle(va("Emerald | UPS: %d FPS: %d", updates, frames));
            timer++;
            updates = frames = 0;
        }
    }
}
void Application::FixedUpdate() {
}

void Application::Update(const TimeStep& time) {
    GetMouse()->Update();
    camera.Update(time);
    sinus += 0.001f * time.GetMills();
    sinus2 += 0.003f * time.GetMills();
    deferred->Update();
}

void Application::Render() {

    m_window->ClearColor(background);
    GetImGuiManager()->Begin();

    deferred->Render();
    GetImGuiManager()->End();

    m_window->SwapBuffers();
    m_window->PollEvents();
    //RenderTest();

    //shader->Start();
    //shader->Set("viewMatrix", camera.GetViewMatrix());
    //shader->Set("eyePos", camera.m_position);
    //
    //shader->Set("ambientIntensity", ambientIntensity);
    //
    //float FOV = ((Math::sin(sinus) + 1) / 2) * 150 + 20;
    ////entity.m_position.x = Math::sin(sinus);
    //entity.m_rotation.x = sinus * 20;
    //entity.m_rotation.y = Math::sin(sinus) * 20;
    ////shader.Set("viewMatrix", camera.GetViewMatrix());
    ////Matrix4 projection = Matrix4::Perspective(FOV, 16.0f / 9.0f, 0.1f, 1000.0f);
    ////shader->Set("projectionMatrix", projection);
    //entity.m_rotation.x = sinus * 180;
    //entity.m_rotation.y = sinus * 90;
    //entity.m_rotation.z = sinus * 360;
    //
    //for (int i = 0; i < 4; i++) {
    //    shader->Set(va("%s%d%s", "lightPosition[", i, "]").c_str(), Vector3(i * 20, i * 20, 2));
    //    shader->Set(va("%s%d%s", "lightColor[", i, "]").c_str(), Color(i*0.25f, 1 - i * 0.25f, 1, 1));
    //    //shader->Set(va("%s%d%s", "attenuation[", i, "]").c_str(), );
    //}
    //
    //shader->Set("lightPosition[0]", camera.m_position);
    //
    //
    //shader->Set("lightColor", directional);
    //shader->Set("direction", position);
    //shader->Set("specularIntensity", specIntensity);
    //shader->Set("specularPower", specPower);
    //
    //shader->Set("tex", texture);
    //shader->Set("texNormal", normal);
    //shader->Set("texHeight", height);
    //shader->Set("color", color);
    //
    //renderer->Begin();
    //Matrix4 transform = Matrix4::Identity();
    //transform *= Matrix4::Translate(Vector3(x, y, z));
    //transform *= Matrix4::Rotate(rotx, Vector3::XAxis());
    //transform *= Matrix4::Rotate(roty, Vector3::YAxis());
    //transform *= Matrix4::Rotate(rotz, Vector3::ZAxis());
    //transform *= Matrix4::Scale(scale);
    //
    ////Matrix4 projection = Matrix4::Perspective(65.0f, 1.778f, 0.01f, 1000.0f);
    ////Matrix4 model = Matrix4::Translate(m_position) * Matrix4::Rotate(m_rotation.x, Vector3(1, 0, 0)) * Matrix4::Rotate(m_rotation.y, Vector3(0, 1, 0)) * Matrix4::Rotate(m_rotation.z, Vector3(0, 0, 1)) * Matrix4::Scale(Vector3(m_scale));
    ////Matrix4 mvp = projection * model;
    //
    //shader->Set("transformationMatrix", transform);
    //
    ////for(int i = 0; i < entities.size(); i++)
    ////{
    ////    entities[i].m_mesh.Draw();
    ////}
    ////entities[0].m_mesh.Draw();
    //
    //for (int x = 0; x < 100; x++) {
    //    for (int y = 0; y < 100; y++) {
    //        float temp = (50 / Math::abs(x - 50) + 50 / Math::abs(y - 50)) / 8;
    //        renderer->Submit(Vector3(x, Math::sin(x + y * 50 + sinus) * var * (temp < 3 ? temp : 3), y));
    //    }
    //}
    //renderer->End();
    //renderer->Draw();
    //
    //shader->Stop();
    //
    //ImGui_ImplOpenGL3_NewFrame();
    //ImGui_ImplGlfw_NewFrame();
    //ImGui::NewFrame();
    //ImGui::Begin("Hello, world!");
    //ImGui::DragFloat("Scale", &scale, 0.001f);
    //ImGui::DragFloat("X", &x, 0.1f);
    //ImGui::DragFloat("Y", &y, 0.1f);
    //ImGui::DragFloat("Z", &z, 0.1f);
    //ImGui::DragFloat("RotX", &rotx, 0.1f);
    //ImGui::DragFloat("RotY", &roty, 0.1f);
    //ImGui::DragFloat("RotZ", &rotz, 0.1f);
    //
    //ImGui::DragFloat3("Position", (float*)&position, 0.01f);
    //ImGui::DragFloat3("Attenuation", (float*)&attenuation, 0.01f);
    //ImGui::DragFloat2("UVScale", (float*)&UVScale, 0.01f);
    //
    //ImGui::SliderInt("Texture", &texture, 0, 3);
    //ImGui::SliderInt("Normal", &normal, 4, 8);
    //ImGui::SliderInt("Height", &height, 0, 8);
    //ImGui::SliderFloat("Var", &var, 0, 10);
    //ImGui::SliderFloat("specIntensity", &specIntensity, 0, 1);
    //ImGui::SliderFloat("specPower", &specPower, 0, 1);
    //ImGui::SliderFloat("Ambient", &ambientIntensity, 0, 1);
    //
    //ImGui::ColorEdit4("Color", (float*)&color);
    //ImGui::ColorEdit4("Directional", (float*)&directional);
    //ImGui::ColorEdit4("Background", (float*)&background);
    //ImGui::End();
    //
    //ImGui::Render();
    //
    //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::Cleanup() {
    delete m_window;
    delete renderer;
    delete m_timer;
    delete m_timeStep;
    delete g_instance;
}