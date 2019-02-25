#pragma once

// Windows Header Files:
#include <windows.h>
#include <time.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <thread>
#include <math.h>
#include <cstdlib>
#include <atomic>
#include <mutex>


// Datastructures
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <vector>
#include <unordered_map>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// ImGui
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imGui/imGuiManager.h"

using namespace std;

#pragma warning (disable: 4267) /*size_t conversions*/

#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix4.h"
#include "math/mathFunctions.h"

#include "util/singleton.h"
#include "util/types.h"
#include "util/thread/threadManager.h"
#include "util/logger.h"
#include "util/color.h"
#include "util/timestep.h"
#include "util/timer.h"
#include "util/utils.h"
#include "util/glError.h"

#include "glCallbackManager.h"

#include "graphics/buffers/bufferLayout.h"
#include "graphics/buffers/buffer.h"
#include "graphics/buffers/indexBuffer.h"
#include "graphics/buffers/vertexArray.h"
#include "graphics/buffers/layoutVertexArray.h"
#include "graphics/buffers/frameBuffer.h"
#include "graphics/buffers/gBuffer.h"

#include "graphics/textures/textureParameters.h"
#include "graphics/textures/texture.h"
#include "graphics/textures/textureUtils.h"
#include "graphics/window.h"
#include "graphics/shaders/shader.h"
#include "graphics/shaders/phongShader.h"
#include "graphics/shaders/UIShader.h"
#include "graphics/renderer.h"

#include "graphics/lighting/pointlight.h"
#include "graphics/lighting/pointlightRenderer.h"

#include "input/keyboard.h"
#include "input/mouse.h"

#include "graphics/camera/camera.h"
#include "graphics/camera/freeCam.h"
#include "graphics/material.h"
#include "graphics/mesh.h"
#include "graphics/model.h"
#include "entity/entity.h"

#include "util/meshGenerator.h"

#include "assets/materialManager.h"
#include "assets/textureManager.h"

#include "deferred.h"
#include "graphics/RenderingPipeline.h"
#include "application.h"
