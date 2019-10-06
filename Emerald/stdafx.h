#pragma once

//#define OTHERWISE else
//#define ASSUMINGTHAT if
//#define CORRECT true
//#define ERRONEOUS false
//#define ANSWER return

#define _HAS_STD_BYTE 0
//#define MEMORY_CHECK
#pragma warning (disable: 4244) /*type_traits conversion warning*/

// Windows Header Files:
#include <windows.h>
#include <time.h>
#include <fileapi.h>

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
#include <iomanip>

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

// Assimp
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include <assimp/Importer.hpp>

#include "vendor/noise.h"
#include "vendor/json.h"
#include "vendor/stb_image.h"

using namespace std;

#pragma warning (disable: 4267) /*size_t conversions*/

#pragma warning (default: 4244) /*size_t conversions*/
#define loop(x, n) for(int x = 0; x < n; ++x)
#define foreach(x, y) for(auto& x : y)
#define BUFFEROFFSET(i) ((char *)NULL + (i))

#include "math/vector2.h"
#include "math/vector2I.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix4.h"
#include "math/rect.h"
#include "math/mathFunctions.h"


#include "util/types.h"
#include "util/ref.h"
#include "util/singleton.h"
#include "imGui/imGuiManager.h"
#include "util/memory.h"
#include "util/fileSystem.h"
#include "util/utils.h"
#include "util/asyncQueue.h"
#include "util/thread/threadManager.h"
#include "util/thread/threadPool.h"
#include "util/logger.h"
#include "util/thread/glFiberManager.h"
#include "util/color.h"
#include "util/timestep.h"
#include "util/timer.h"
#include "util/glError.h"

#include "math/groundRaycast.h"

#undef DELETE

#ifdef MEMORY_CHECK
#define NEW(x) new (typeid(x).name()) x
#define NEWARRAY(x, y) new (typeid(x).name()) x[y]
#define DELETE(x) Destroy(x)

#else
#define NEW(x) new x
#define NEWARRAY(x, y) new x[y]
#define DELETE(x) if(x) delete x
#define DELETEARRAY(x) delete[] x

#endif

//#define DELETEARRAY(x) DestroyArray(x)

#include "util/tween/tween.h"

#include "glCallbackManager.h"

#include "assets/texture/textureParameters.h"

#include "graphics/buffers/bufferLayout.h"
#include "graphics/buffers/vertexBuffer.h"
#include "graphics/buffers/indexBuffer.h"
#include "graphics/buffers/vertexArray.h"
#include "graphics/buffers/frameBuffer.h"
#include "graphics/buffers/gBuffer.h"

#include "graphics/shaders/shaderUniformBuffer.h"
#include "graphics/shaders/shader.h"
#include "graphics/shaders/shaderManager.h"
#include "graphics/vertex.h"

#include "assets/assetBase.h"
#include "assets/texture/texture.h"
#include "assets/material/material.h"
#include "assets/material/basicMaterial.h"
#include "assets/material/tileMaterial.h"
#include "assets/texture/textureArray.h"
#include "assets/texture/textureUtils.h"
#include "assets/texture/tileMaterialManager.h"
#include "assets/texture/icon.h"
#include "assets/loaders/assetLoader.h"
#include "assets/loaders/customLoader.h"
#include "assets/loaders/textureLoader.h"
#include "assets/loaders/tileTextureLoader.h"
#include "assets/loaders/shaderLoader.h"
#include "assets/loaders/modelLoader.h"
#include "assets/loaders/stateLoader.h"

#include "graphics/window.h"
#include "graphics/shaders/UIRenderer.h"
#include "graphics/renderer.h"
#include "graphics/ssaoRenderer.h"

#include "input/keyboard.h"
#include "input/mouse.h"

#include "graphics/camera/freeCam.h"
#include "graphics/camera/firstPersonCam.h"
#include "graphics/mesh.h"
#include "graphics/customMesh.h"
#include "graphics/model.h"
#include "entity/entity.h"
#include "assets/assetBatch.h"
#include "assets/basicAssetBatch.h"
#include "assets/assetManager.h"
#include "assets/assetWatcher.h"

#include "graphics/instancedRenderer.h"
#include "graphics/lineRenderer.h"
#include "graphics/tileRenderer.h"

#include "graphics/lighting/directionalLight.h"
#include "graphics/lighting/pointlight.h"
#include "graphics/lighting/pointlightRenderer.h"

#include "util/meshGenerator.h"

#include "world/tile.h"
#include "world/tileGrid.h"
#include "world/world.h"

#include "assets/materialManager.h"
#include "assets/textureManager.h"

#include "deferred.h"
#include "graphics/RenderingPipeline.h"
#include "application.h"

#include "physics/rigidBody.h"
#include "physics/player.h"

#include "util/ludo/aStar.h"

#include "state/state.h"
#include "state/stateManager.h"
#include "state/states/loadingState.h"
#include "state/states/menuState.h"
#include "state/states/gameState.h"
#include "state/states/pauseState.h"
#include "state/states/gameOverState.h"
