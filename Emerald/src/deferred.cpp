 #include "stdafx.h"
// 
// GBuffer* gBuffer;
// 
// Shader* hdrShader;
// Shader* outputGeoShader;
// Shader* directionalLightShader;
// Shader* pointLightShader;
// 
// Vector3 directional(-0.7, 0.3, 0.1);
// FreeCam* cam;
// FrameBuffer* hdr;
// struct Light {
//     float sinval1;
//     float sinval2;
//     float sinval3;
//     float sinincrement1;
//     float sinincrement2;
//     float sinincrement3;
//     Vector3 m_pos;
//     Vector3 m_original;
//     Color m_col;
//     float m_radius;
// 
// };
// 
// Light* currentLight;
// vector<Light> lights;
// Texture* header;
// 
// UIShader* uiShader;
// Mesh* quad;
// Model model;
// Model model2;
// Mesh* sphere;
// 
// PointlightRenderer* renderer;
// vector<Pointlight> pointLights;
// 
// void CreateSphere() {
//     int stacks = 20;
//     int slices = 20;
//     const float PI = 3.14f;
// 
//     std::vector<float> positions;
//     std::vector<GLuint> indices;
// 
//     // loop through stacks.
//     for (int i = 0; i <= stacks; ++i) {
// 
//         float V = (float)i / (float)stacks;
//         float phi = V * PI;
// 
//         // loop through the slices.
//         for (int j = 0; j <= slices; ++j) {
// 
//             float U = (float)j / (float)slices;
//             float theta = U * (PI * 2);
// 
//             // use spherical coordinates to calculate the positions.
//             float x = cos(theta) * sin(phi);
//             float y = cos(phi);
//             float z = sin(theta) * sin(phi);
// 
//             positions.push_back(x);
//             positions.push_back(y);
//             positions.push_back(z);
//         }
//     }
// 
//     // Calc The Index Positions
//     for (int i = 0; i < slices * stacks + slices; ++i) {
//         indices.push_back(GLuint(i));
//         indices.push_back(GLuint(i + slices + 1));
//         indices.push_back(GLuint(i + slices));
// 
//         indices.push_back(GLuint(i + slices + 1));
//         indices.push_back(GLuint(i));
//         indices.push_back(GLuint(i + 1));
//     }
// 
//     VertexArray* sphereVAO = new VertexArray();
//     sphereVAO->AddBuffer(new Buffer(positions.data(), positions.size(), 3), 0, false);
// 
//     IndexBuffer* sphereIBO = new IndexBuffer(indices.data(), indices.size());
//     sphere = new Mesh(sphereVAO, sphereIBO);
// 
//     renderer = new PointlightRenderer(sphere);
// }
// 
// void CreateQuad() {
//     GLfloat vertices[] = { -1, -1, 0,
//                            -1,  1, 0,
//                             1,  1, 0,
//                             1, -1, 0 };
//     uint indices[] = { 0, 1, 2, 0, 2, 3 };
// 
//     VertexArray* m_vao = new VertexArray();
//     m_vao->AddBuffer(new Buffer(vertices, NUMOF(vertices), 3), 0, false);
// 
//     quad = new Mesh(m_vao, new IndexBuffer(indices, NUMOF(indices)));
// }
// 
// void SetupDeferredShader(Shader* shader) {
//     gBuffer->BindTextures();
//     shader->Set("uColorTex", 0);
//     shader->Set("uNormalTex", 1);
//     shader->Set("uPositionTex", 2);
//     shader->Set("uCameraPos", cam->m_position);
// }
// 
// void RenderPointLight(float radius, const Vector3& position, const Color& color) {
//     pointLightShader->Set("uLightRadius", radius);
//     pointLightShader->Set("uLightPosition", position);
//     pointLightShader->Set("uLightColor", color.R, color.G, color.B);
//     sphere->Draw();
//     //glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
// }
// 
// void NewLight() {
//     Light light;
//     light.m_col = Color::Random();
//     light.m_pos = Vector3();
//     light.m_radius = 5;
//     lights.push_back(light);
//     currentLight = &lights.back();
// }
// 
// Vector2 pos(0, 0);
// Vector2 sizee(1, 1);
// Vector2 origin(0, 0);
// float rot = 0;
// bool x, y;
// 
// void Deferred::Resize(uint width, uint height) {
//     if (gBuffer) {
//         //delete gBuffer;
//         //gBuffer = new GBuffer(width, height);
//     }
//     if (hdr) {
//         //delete hdr;
//         //hdr = new FrameBuffer(width, height);
//     }
// }
// 
// void Deferred::Initialize(Window* window, FreeCam& camera) {
//     int val = 16;
//     for (int x = -val; x < val; x++) {
//         for (int y = -val; y < val; y++) {
//             for (int z = -val; z < val; z++) {
//                 Light light;
//                 light.sinval1 = 0;
//                 light.sinval2 = 0;
//                 light.sinval3 = 0;
//                 light.sinincrement1 = Math::RandomFloat(0, 0.1f);
//                 light.sinincrement2 = Math::RandomFloat(0, 0.1f);
//                 light.sinincrement3 = Math::RandomFloat(0, 0.1f);
//                 light.m_pos = Vector3(x * 100, y * 50 + 50, z * 50);
//                 light.m_original = light.m_pos;
//                 light.m_col = Color::RandomPrimary();
//                 light.m_radius = 200;
//                 lights.push_back(light);
//                 pointLights.push_back(Pointlight(light.m_pos, Math::RandomFloat(10, 50), light.m_col));
//             }
//         }
//     }
//     cam = &camera;
//     m_window = window;
// 
//     hdrShader = new Shader("HDR", "src/shader/hdr.vert", "src/shader/hdr.frag");
//     outputGeoShader = new Shader("Geo", "src/shader/geoVert.glsl", "src/shader/geoFrag.glsl");
//     directionalLightShader = new Shader("Directional", "src/shader/directionalVert.glsl", "src/shader/directionalFrag.glsl");
//     pointLightShader = new Shader("Pointlight", "src/shader/pointlightVert.glsl", "src/shader/pointlightFrag.glsl");
// 
//     gBuffer = new GBuffer(1920, 1080);
// 
//     model.LoadModel("sponza/sponza.obj");
//     //model2.LoadModel("fern/2.obj");
//     //Texture* tex = new Texture("fern/normal.png");
//     //Texture* tex2 = new Texture("fern/fern.png");
//     //model2.GetMeshes().at(0)->GetMaterial()->SetAlbedo(tex2);
//     //model2.GetMeshes().at(0)->GetMaterial()->SetNormal(tex);
// 
//     uiShader = new UIShader();
//     uiShader->Initialize();
// 
//     header = new Texture("header.png");
//     sizee.x = (float)header->GetWidth() / GetApplication()->GetWindow()->GetWidth();
//     sizee.y = (float)header->GetHeight() / GetApplication()->GetWindow()->GetHeight();
//     CreateSphere();
//     CreateQuad();
//     NewLight();
//     //hdr = new FrameBuffer(1920, 1080);
// }
// 
// bool lightEnabled = true;
// void Deferred::Update() {
//     if (lightEnabled && ButtonJustDown(VK_MOUSE_LEFT)) {
//         NewLight();
//     }
// 
//     if (lightEnabled) Utils::setPositionInFrontOfCam(currentLight->m_pos, *cam, 0.22f);
// }
// 
// void Deferred::Render() {
// 
//     for (Light& l : lights) {
//         l.sinval1 += l.sinincrement1;
//         l.sinval2 += l.sinincrement2;
//         l.sinval3 += l.sinincrement3;
//         l.m_pos.x += Math::Sin(l.sinval1);
//         l.m_pos.y += Math::Sin(l.sinval2);
//         l.m_pos.z += Math::Sin(l.sinval3);
//     }
// 
//     static float sin = 0.00;
//     sin += 0.002f;
//     for (int i = 0; i < pointLights.size(); i++) {
//         Pointlight& light = pointLights[i];
//         Light& l = lights[i];
//         l.sinval1 += l.sinincrement1;
//         l.sinval2 += l.sinincrement2;
//         l.sinval3 += l.sinincrement3;
// 
//         light.m_position.x += Math::Sin(l.sinval1);
//         light.m_position.y += Math::Sin(l.sinval2);
//         light.m_position.z += Math::Sin(l.sinval3);
//     }
// 
//     Matrix4 projectionMatrix = Matrix4::Perspective(70, (float)(1920) / 1080, 0.1f, 3000.0f);
// 
//     // setup GL state.
//     GL(glEnable(GL_DEPTH_TEST));
//     GL(glDepthMask(true));
//     GL(glDisable(GL_BLEND));
//     GL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
//     GL(glEnable(GL_CULL_FACE));
//     GL(glFrontFace(GL_CCW));
// 
//     gBuffer->Bind();
//     gBuffer->Clear();
// 
//     outputGeoShader->Bind();
//     outputGeoShader->Set("projectionMatrix", projectionMatrix);
//     outputGeoShader->Set("viewMatrix", cam->GetViewMatrix());
// 
//     outputGeoShader->Set("uDiffTex", 0);
//     outputGeoShader->Set("uBumpTex", 1);
// 
//     model.Draw(outputGeoShader);
// 
//     gBuffer->Unbind();
// 
//     //
//     // Now comes the Deferred shading!
//     //
//     GL(glViewport(0, 0, GetApplication()->GetWindow()->GetWidth(), GetApplication()->GetWindow()->GetHeight()));
//     GL(glClearColor(0.0f, 0.0f, 0.3f, 1.0f));
//     GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
// 
//     hdr->Bind();
//     hdr->Clear();
//     //
//     // first, we render a single directional light, with a fullscreen pass.
//     //
//     directionalLightShader->Bind();
//     SetupDeferredShader(directionalLightShader);
//     directionalLightShader->Set("directional", directional);
//     // we use attribute-less rendering to render a full-screen triangle.
//     // so the triangle vertices are basically stored in the vertex shader.
//     // see the vertex shader for more details.
//     GL(glDrawArrays(GL_TRIANGLES, 0, 3));
// 
//     // Next, we render all the point light soures.
//     // We will be doing our own depth testing in frag shader, so disable depth testing.
//     // Enable alpha blending. So that the rendered point lights are added to the framebuffer.
//     //
//     GL(glDisable(GL_DEPTH_TEST));
//     GL(glEnable(GL_BLEND));
//     GL(glBlendFunc(GL_ONE, GL_ONE));
// 
//     // We render only the inner faces of the light sphere.
//     // In other words, we render the back-faces and not the front-faces of the sphere.
//     // If we render the front-faces, the lighting of the light sphere disappears if
//     // we are inside the sphere, which is weird. But by rendering the back-faces instead,
//     // we solve this problem.
//     GL(glFrontFace(GL_CW));
// 
//     pointLightShader->Reload();
// 
//     pointLightShader->Bind();
//     SetupDeferredShader(pointLightShader);
//     pointLightShader->Set("projectionMatrix", projectionMatrix);
//     pointLightShader->Set("viewMatrix", cam->GetViewMatrix());
//     // We render every point light as a light sphere. And this light sphere is added onto the framebuffer
//     // with additive alpha blending.
// 
//     renderer->Draw(pointLights);
// 
//     //for (auto & light : lights) {
//     //    RenderPointLight(light.m_radius, light.m_pos, light.m_col);
//     //}
// 
//     hdr->Unbind();
// 
//     GL(glDisable(GL_BLEND));
// 
//     hdrShader->Reload();
// 
//     hdrShader->Bind();
//     glActiveTexture(GL_TEXTURE0);
//     //glBindTexture(GL_TEXTURE_2D, hdr->GetTexture()->GetHandle());
//     quad->Draw();
// 
//     //uiShader->RenderTexture(hdr->GetTexture()->GetHandle(), origin, rot, Vector2(0, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), x, y);
//     //uiShader->RenderTexture(gBuffer->m_colorTexture, origin, 0, Vector2(0.25f, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), x, y);
//     //uiShader->RenderTexture(gBuffer->m_normalTexture, origin, 0, Vector2(0.5f, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), x, y);
//     //uiShader->RenderTexture(gBuffer->m_positionTexture, origin, 0, Vector2(0.75f, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), x, y);
// 
//     ImGui::Begin("Hello, world!");
//     ImGui::Checkbox("x", &x);
//     ImGui::Checkbox("y", &y);
//     ImGui::DragFloat("Rot", (float*)&rot, 0.01f);
//     ImGui::DragFloat2("Pos", (float*)&pos, 0.01f);
//     ImGui::DragFloat2("Origin", (float*)&origin, 0.01f, 0, 1);
//     ImGui::DragFloat2("Size", (float*)&sizee, 0.01f);
//     ImGui::DragFloat3("Directional", (float*)&directional, 0.01f);
//     ImGui::DragFloat("Radius", (float*)&currentLight->m_radius, 0.5f);
//     ImGui::DragFloat3("Pos", (float*)&currentLight->m_pos, 0.01f);
//     ImGui::ColorEdit4("Color", (float*)&currentLight->m_col);
//     ImGui::Checkbox("Enabled", &lightEnabled);
// 
//     ImGui::End();     
// }