#include "stdafx.h"

GLuint colorTexture;
GLuint depthRenderbuffer;
GLuint normalTexture;
GLuint positionTexture;

GLuint fbo;
GLuint vao;

Shader* outputGeoShader;
Shader* directionalLightShader;
Shader* pointLightShader;

Vector3 directional(-0.7, 0.3, 0.1);
int fbWidth, fbHeight;
FreeCam* cam;

struct Light {
    float sinval1;
    float sinval2;
    float sinval3;
    float sinincrement1;
    float sinincrement2;
    float sinincrement3;
    Vector3 m_pos;
    Color m_col;
    float m_radius;
};

Light* currentLight;
vector<Light> lights;

#define GL_C(stmt) stmt;


Model model;
Model model2;
Mesh* sphere;
void CreateSphere() {
    int stacks = 20;
    int slices = 20;
    const float PI = 3.14f;

    std::vector<float> positions;
    std::vector<GLuint> indices;

    // loop through stacks.
    for (int i = 0; i <= stacks; ++i) {

        float V = (float)i / (float)stacks;
        float phi = V * PI;

        // loop through the slices.
        for (int j = 0; j <= slices; ++j) {

            float U = (float)j / (float)slices;
            float theta = U * (PI * 2);

            // use spherical coordinates to calculate the positions.
            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);
        }
    }

    // Calc The Index Positions
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

    IndexBuffer* sphereIBO = new IndexBuffer(indices.data(), indices.size());
    sphere = new Mesh(sphereVAO, sphereIBO);
}

// configure a shader for usage in deferred rendering.
void SetupDeferredShader(Shader* shader) {
    // bind gbuffer textures.
    shader->Set("uColorTex", 0);
    GL_C(glActiveTexture(GL_TEXTURE0 + 0));
    GL_C(glBindTexture(GL_TEXTURE_2D, colorTexture));

    shader->Set("uNormalTex", 1);
    GL_C(glActiveTexture(GL_TEXTURE0 + 1));
    GL_C(glBindTexture(GL_TEXTURE_2D, normalTexture));

    shader->Set("uPositionTex", 2);
    GL_C(glActiveTexture(GL_TEXTURE0 + 2));
    GL_C(glBindTexture(GL_TEXTURE_2D, positionTexture));

    shader->Set("uCameraPos", cam->m_position);
}

void RenderPointLight(float radius, const Vector3& position, const Color& color) {
    pointLightShader->Set("uLightRadius", radius);
    pointLightShader->Set("uLightPosition", position);
    pointLightShader->Set("uLightColor", color.R, color.G, color.B);
    sphere->Draw();
    //glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
}

void LoadModel(void) {
    model.LoadModel("sponza/sponza.obj");
    //model2.LoadModel("fern/2.obj");
    //Texture* tex = new Texture("fern/normal.png");
    //Texture* tex2 = new Texture("fern/fern.png");
    //model2.GetMeshes().at(0)->GetMaterial()->SetAlbedo(tex2);
    //model2.GetMeshes().at(0)->GetMaterial()->SetNormal(tex);
}

void NewLight() {
    Light light;
    light.m_col = Color::Random();
    light.m_pos = Vector3();
    light.m_radius = 5;
    lights.push_back(light);
    currentLight = &lights.back();
}

void Deferred::Initialize(Window* window, FreeCam& camera) {
    int val = 8;
    for (int x = -val; x < val; x++) {
        for (int y = -val; y < val; y++) {
            for (int z = -val; z < val; z++) {
                Light light;
                light.sinval1 = 0;
                light.sinval2 = 0;
                light.sinval3 = 0;
                light.sinincrement1 = Math::RandomF(0, 0.1f);
                light.sinincrement2 = Math::RandomF(0, 0.1f);
                light.sinincrement3 = Math::RandomF(0, 0.1f);
                light.m_pos = Vector3(x * 200, y * 200, z * 200);
                light.m_col = Color::Random();
                light.m_radius = 200;
                lights.push_back(light);
            }
        }

    }
    cam = &camera;
    m_window = window;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glfwGetFramebufferSize(window->GetWindow(), &fbWidth, &fbHeight);

    outputGeoShader = new Shader("Geo", "src/shader/geoVert.glsl", "src/shader/geoFrag.glsl");
    directionalLightShader = new Shader("Directional", "src/shader/directionalVert.glsl", "src/shader/directionalFrag.glsl");
    pointLightShader = new Shader("Pointlight", "src/shader/pointlightVert.glsl", "src/shader/pointlightFrag.glsl");

    //GBUFFER
    // create the gbuffer. first create fbo:
    GL_C(glGenFramebuffers(1, &fbo));
    GL_C(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

    // RGBA8 color texture-p
    GL_C(glGenTextures(1, &colorTexture));
    GL_C(glBindTexture(GL_TEXTURE_2D, colorTexture));
    GL_C(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fbWidth, fbHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    GL_C(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_C(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_C(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, colorTexture, 0));
    GL_C(glBindTexture(GL_TEXTURE_2D, 0));

    //  RGBA16F normal texture.
    GL_C(glGenTextures(1, &normalTexture));
    GL_C(glBindTexture(GL_TEXTURE_2D, normalTexture));
    GL_C(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, fbWidth, fbHeight, 0, GL_RGBA, GL_FLOAT, NULL));
    GL_C(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_C(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_C(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
        GL_TEXTURE_2D, normalTexture, 0));
    GL_C(glBindTexture(GL_TEXTURE_2D, 0));

    //  RGBA16F position texture.
    GL_C(glGenTextures(1, &positionTexture));
    GL_C(glBindTexture(GL_TEXTURE_2D, positionTexture));
    GL_C(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, fbWidth, fbHeight, 0, GL_RGBA, GL_FLOAT, NULL));
    GL_C(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_C(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_C(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
        GL_TEXTURE_2D, positionTexture, 0));
    GL_C(glBindTexture(GL_TEXTURE_2D, 0));

    // we need a z-buffer for the gbuffer. but we don't need to read from it.
    // so instead create a renderbuffer.
    GL_C(glGenRenderbuffers(1, &depthRenderbuffer));
    GL_C(glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer));
    GL_C(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, fbWidth, fbHeight));
    GL_C(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    GL_C(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer));

    // specify that we can render to all three attachments.
    // this is very important! It won't work otherwise.
    GLenum tgts[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    GL_C(glDrawBuffers(3, tgts));

    // make sure nothing went wrong:
    GLenum status;
    GL_C(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer not complete. Status: %d", status);
        exit(1);
    }
    GL_C(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    // gbuffer done!

    LoadModel(); // load the obj model.
    CreateSphere(); // create the light sphere geometry.
    NewLight();
}

bool lightEnabled = true;
void Deferred::Update() {
    if (lightEnabled && ButtonJustDown(VK_MOUSE_LEFT)) {
        NewLight();
    }

    if (lightEnabled) Utils::setPositionInFrontOfCam(currentLight->m_pos, *cam, 0.22f);
}

void Deferred::Render() {
    for(Light& l : lights)
    {
        l.sinval1 += l.sinincrement1;
        l.sinval2 += l.sinincrement2;
        l.sinval3 += l.sinincrement3;
        l.m_pos.x += Math::sin(l.sinval1);
        l.m_pos.y += Math::sin(l.sinval2);
        l.m_pos.z += Math::sin(l.sinval3);
    }
    // setup matrices.
    Matrix4 projectionMatrix = Matrix4::Perspective(70, (float)(1920) / 1080, 0.1f, 3000.0f);
    //Matrix4 viewMatrix = camera.GetViewMatrix();
    //Matrix4 VP = viewMatrix;

    // setup GL state.
    GL_C(glEnable(GL_DEPTH_TEST));
    GL_C(glDepthMask(true));
    GL_C(glDisable(GL_BLEND));
    GL_C(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    GL_C(glEnable(GL_CULL_FACE));
    GL_C(glFrontFace(GL_CCW));

    //
    // In the first pass, we just write to the gbuffer.
    //
    GL_C(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo)); // bind g buffer for writing.

    GL_C(glViewport(0, 0, fbWidth, fbHeight));
    GL_C(glClearColor(0.0f, 0.0f, 0.3f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    outputGeoShader->Start();
    outputGeoShader->Set("projectionMatrix", projectionMatrix);
    outputGeoShader->Set("viewMatrix", cam->GetViewMatrix());
    //GL_C(glUniformMatrix4fv(glGetUniformLocation(outputGeoShader, "transformationMatrix"), 1, GL_FALSE, (GLfloat *)VP.elements));

    outputGeoShader->Set("uDiffTex", 0);
    outputGeoShader->Set("uBumpTex", 1);

    model.Draw(outputGeoShader);
    //model.Draw(outputGeoShader);
    // now we render all the meshes, one after one.
    //for (Meshh* mesh : meshes) {
    //    //
    //    // setup vertex attribs.
    //    //
    //    GL_C(glEnableVertexAttribArray(0));
    //    GL_C(glBindBuffer(GL_ARRAY_BUFFER, mesh->positionVbo));
    //    GL_C(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    //
    //    GL_C(glEnableVertexAttribArray(1));
    //    GL_C(glBindBuffer(GL_ARRAY_BUFFER, mesh->normalVbo));
    //    GL_C(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    //
    //    GL_C(glEnableVertexAttribArray(2));
    //    GL_C(glBindBuffer(GL_ARRAY_BUFFER, mesh->uvVbo));
    //    GL_C(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
    //
    //    // enable texture.
    //
    //    Material mat = materials[mesh->matId];
    //    if (mat.diffuseTexFile != "") {
    //        GL_C(glBindTexture(GL_TEXTURE_2D, mat.diffuseTex));
    //        GL_C(glActiveTexture(GL_TEXTURE0));
    //        outputGeoShader->Set("uDiffTex", 0);
    //    }
    //
    //    glDrawArrays(GL_TRIANGLES, 0, mesh->positions.size());
    //}

    GL_C(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0)); // stop writing to gbuffer.

    //
    // Now comes the Deferred shading!
    //
    GL_C(glViewport(0, 0, fbWidth, fbHeight));
    GL_C(glClearColor(0.0f, 0.0f, 0.3f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    //
    // first, we render a single directional light, with a fullscreen pass.
    //
    directionalLightShader->Start();
    SetupDeferredShader(directionalLightShader);
    directionalLightShader->Set("directional", directional);
    // we use attribute-less rendering to render a full-screen triangle.
    // so the triangle vertices are basically stored in the vertex shader.
    // see the vertex shader for more details.
    GL_C(glDrawArrays(GL_TRIANGLES, 0, 3));

    //
    // Next, we render all the point light soures.
    // We will be doing our own depth testing in frag shader, so disable depth testing.
    // Enable alpha blending. So that the rendered point lights are added to the framebuffer.
    //
    GL_C(glDisable(GL_DEPTH_TEST));
    GL_C(glEnable(GL_BLEND));
    GL_C(glBlendFunc(GL_ONE, GL_ONE));

    // We render only the inner faces of the light sphere.
    // In other words, we render the back-faces and not the front-faces of the sphere.
    // If we render the front-faces, the lighting of the light sphere disappears if
    // we are inside the sphere, which is weird. But by rendering the back-faces instead,
    // we solve this problem.
    GL_C(glFrontFace(GL_CW));

    pointLightShader->Start();
    SetupDeferredShader(pointLightShader);
    pointLightShader->Set("projectionMatrix", projectionMatrix);
    pointLightShader->Set("viewMatrix", cam->GetViewMatrix());
    // We render every point light as a light sphere. And this light sphere is added onto the framebuffer
    // with additive alpha blending.

    for (auto & light : lights) {
        RenderPointLight(light.m_radius, light.m_pos, light.m_col);
    }

    ImGui::Begin("Hello, world!");
    ImGui::DragFloat3("Directional", (float*)&directional, 0.01f);
    ImGui::DragFloat("Radius", (float*)&currentLight->m_radius, 0.5f);
    ImGui::DragFloat3("Pos", (float*)&currentLight->m_pos, 0.01f);
    ImGui::ColorEdit4("Color", (float*)&currentLight->m_col);
    ImGui::Checkbox("Enabled", &lightEnabled);

    ImGui::End();
}