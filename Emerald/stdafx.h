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

// Datastructures
#include <map>
#include <set>
#include <stack>
#include <vector>
#include <unordered_map>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// ImGui
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix4.h"
#include "math/mathFunctions.h"
#include "util/logger.h"
#include "util/color.h"
#include "util/types.h"
#include "util/utils.h"

#include "graphics/buffers/buffer.h"
#include "graphics/buffers/indexBuffer.h"
#include "graphics/buffers/VertexArray.h"

#include "graphics/texture.h"
#include "graphics/window.h"
#include "graphics/shaders/shader.h"
#include "graphics/shaders/basicShader.h"

#include "graphics/camera/camera.h"
#include "graphics/mesh.h"
#include "entity/entity.h"

#include "application.h"
