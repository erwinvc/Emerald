#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _HAS_STD_BYTE 0
#define NOGDI


//#define MEMORY_CHECK
#pragma warning (disable: 4244) /*type_traits conversion warning*/

// Windows Header Files:
#include <windows.h>
#include <time.h>
#include <fileapi.h>

#include <limits>
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

//Networking
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#pragma comment(lib, "Ws2_32.lib")

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
#include "vendor/shadInclude.h"

using namespace std;

#pragma warning (disable: 4267) /*size_t conversions*/

#pragma warning (default: 4244) /*size_t conversions*/
#define loop(x, n) for(int x = 0; x < n; ++x)
#define foreach(x, y) for(auto& x : y)
#define BUFFEROFFSET(i) ((char *)NULL + (i))

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"
#include "vendor/glm/gtc/noise.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/matrix_access.hpp"
#include "vendor/glm/gtx/transform.hpp"
#include "vendor/glm/gtx/quaternion.hpp"
#include "vendor/glm/gtx/compatibility.hpp"
#include "vendor/glm/gtx/euler_angles.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "vendor/glm/gtx/hash.hpp"

#include "util/types.h"

#include "math/rectangle.h"
#include "math/random.h"
#include "math/mathFunctions.h"
#include "math/blockIterator.h"
#include "math/transform.h"
#include "math/coordinates.h"
#include "math/aabb.h"

#include "util/color.h"
#include "util/ref.h"
#include "util/singleton.h"
#include "util/memory.h"
#include "util/fileSystem.h"
#include "util/utils.h"
#include "imGui/imGuiManager.h"
#include "util/asyncQueue.h"
#include "util/thread/threadManager.h"
#include "util/thread/threadPool.h"
#include "util/logger.h"
#include "util/path.h"
#include "util/thread/glFiberManager.h"
#include "util/timestep.h"
#include "util/timer.h"
#include "util/glError.h"

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
#include "util/profiler.h"

#include "glCallbackManager.h"

//#include "networking/winsock.h"
//#include "networking/udpListener.h"
//#include "networking/socket.h"
#include "network/network.h"
#include "network/packets.h"
#include "network/clientSession.h"

#include "assets/texture/textureParameters.h"

#include "graphics/buffers/bufferLayout.h"
#include "graphics/buffers/vertexBuffer.h"
#include "graphics/buffers/vertexArray.h"
#include "graphics/buffers/frameBuffer.h"
#include "graphics/buffers/gBuffer.h"

#include "graphics/shaders/shaderUniformBuffer.h"
#include "graphics/shaders/shaderProgram.h"
#include "graphics/shaders/shaderProperties.h"
#include "graphics/shaders/shader.h"
#include "graphics/shaders/shaderManager.h"
#include "graphics/shaders/materialCallback.h"

#include "graphics/buffers/uniformBuffer.h"

#include "assets/assetBase.h"
#include "assets/texture/texture.h"
#include "assets/texture/sprite.h"
#include "assets/material/material.h"
#include "assets/material/basicMaterial.h"
#include "assets/material/tileMaterial.h"
#include "assets/texture/textureArray.h"
#include "assets/texture/textureUtils.h"
#include "assets/texture/blockTextureArrayManager.h"
#include "assets/texture/icon.h"
#include "assets/loaders/assetLoader.h"
#include "assets/loaders/customLoader.h"
#include "assets/loaders/textureLoader.h"
#include "assets/loaders/pbrTextureLoader.h"
#include "assets/loaders/blockTextureArrayLoader.h"
#include "assets/loaders/shaderLoader.h"
#include "assets/loaders/modelLoader.h"
#include "assets/loaders/stateLoader.h"

#include "input/keyboard.h"
#include "input/mouse.h"

#include "math/ray.h"
#include "math/groundRaycast.h"
#include "graphics/camera/orthoCamera.h"
#include "graphics/camera/freeCam.h"
#include "graphics/camera/firstPersonCam.h"
#include "math/rasterization.h"

#include "graphics/window.h"
#include "graphics/renderer.h"

#include "graphics/ambientOcclusionRenderer.h"
#include "graphics/ssrRenderer.h"
#include "graphics/bloomRenderer.h"

#include "graphics/mesh.h"
#include "graphics/customMesh.h"
#include "graphics/model.h"
#include "entity/entity.h"
#include "assets/assetBatch.h"
#include "assets/basicAssetBatch.h"
#include "assets/assetManager.h"
#include "assets/assetWatcher.h"

#include "graphics/depthCubemap.h"
#include "graphics/directionalShadow.h"
#include "graphics/shadowMapGenerator.h"
#include "graphics/elementsRenderer.h"
#include "graphics/instancedRenderer.h"
#include "graphics/lineRenderer.h"
#include "graphics/tileRenderer.h"
#include "graphics/spriteRenderer.h"


#include "graphics/lighting/directionalLight.h"
#include "graphics/lighting/pointlight.h"
#include "graphics/lighting/pointlightRenderer.h"

#include "util/meshGenerator.h"

#include "assets/materialManager.h"
#include "assets/textureManager.h"

#include "deferred.h"
#include "graphics/pipeline/HDRPipeline.h"

#include "util/ludo/aStar.h"

#include "world/tile.h"
#include "world/tileGrid.h"
#include "world/coordinateSystem.h"
#include "world/chunkMesh.h"
#include "world/chunkMeshGenerator.h"
#include "world/chunk.h"
#include "world/chunkManager.h"
#include "world/clientWorld.h"

//#include "physics/rigidBody.h"

#include "client.h"
#include "server/serverWorld.h"
#include "server/server.h"
#include "application.h"

#include "state/state.h"
#include "state/stateManager.h"
#include "state/states/editorState.h"
#include "state/states/loadingState.h"
#include "state/states/menuState.h"
#include "state/states/gameState.h"
#include "state/states/pauseState.h"
#include "state/states/voxelState.h"
