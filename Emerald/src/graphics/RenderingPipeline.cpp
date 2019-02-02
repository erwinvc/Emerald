#include "stdafx.h"

void RenderingPipeline::Initialize() {
    //Deferred
    //#Dirty get window size from config?
    m_gBuffer = new GBuffer(1920, 1080);
    m_gBuffer->SetClearColor(Color(0.0f, 0.0f, 0.3f, 1.0f));

    //#Dirty add proper shader asset loading
    m_geometryShader = new Shader("Geo", "src/shader/geoVert.glsl", "src/shader/geoFrag.glsl");
    m_directionalLightShader = new Shader("Directional", "src/shader/directionalVert.glsl", "src/shader/directionalFrag.glsl");
    m_pointLightShader = new Shader("Pointlight", "src/shader/pointlightVert.glsl", "src/shader/pointlightFrag.glsl");

    //HDR
    m_hdrBuffer = new FrameBuffer(1920, 1080, Color(0.0f, 0.0f, 0.3f, 1.0f));
    m_hdrShader = new Shader("HDR", "src/shader/hdr.vert", "src/shader/hdr.frag");

    //UI
    m_uiShader = new UIShader();
    m_uiShader->Initialize();

    m_projectionMatrix = Matrix4::Perspective(70, (float)(1920) / 1080, 0.1f, 3000.0f);

    //Shader variables
    m_geometryShader->Set("projectionMatrix", m_projectionMatrix);
    m_geometryShader->Set("uDiffTex", 0);
    m_geometryShader->Set("uBumpTex", 1);

    m_directionalLightShader->Set("uColorTex", 0);
    m_directionalLightShader->Set("uNormalTex", 1);
    m_directionalLightShader->Set("uPositionTex", 2);
    m_pointLightShader->Set("uColorTex", 0);
    m_pointLightShader->Set("uNormalTex", 1);
    m_pointLightShader->Set("uPositionTex", 2);

}

void RenderingPipeline::Render() {
    GL(glEnable(GL_DEPTH_TEST));
    GL(glDepthMask(true));
    GL(glDisable(GL_BLEND));
    GL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    GL(glEnable(GL_CULL_FACE));
    GL(glFrontFace(GL_CCW));

    //Draw to gBuffer
    m_gBuffer->Bind();
    m_gBuffer->Clear();

    m_geometryShader->Set("viewMatrix", m_camera->GetViewMatrix());

    RenderGeometry();

    m_gBuffer->Unbind();

    //Draw to HDR
    m_hdrBuffer->Bind();
    m_hdrBuffer->Clear();

    m_directionalLightShader->Bind();
    m_gBuffer->BindTextures();

    m_directionalLightShader->Set("uCameraPos", m_camera->m_position);
    m_directionalLightShader->Set("directional", m_directionalLight);
    GL(glDrawArrays(GL_TRIANGLES, 0, 3));

    //
    // Next, we render all the point light soures.
    // We will be doing our own depth testing in frag shader, so disable depth testing.
    // Enable alpha blending. So that the rendered point lights are added to the framebuffer.
    //
    GL(glDisable(GL_DEPTH_TEST));
    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_ONE, GL_ONE));

    // We render only the inner faces of the light sphere.
    // In other words, we render the back-faces and not the front-faces of the sphere.
    // If we render the front-faces, the lighting of the light sphere disappears if
    // we are inside the sphere, which is weird. But by rendering the back-faces instead,
    // we solve this problem.
    GL(glFrontFace(GL_CW));


}

void RenderingPipeline::RenderGeometry() {

}

void RenderingPipeline::Resize(uint width, uint height) {

}
