#pragma once //Emerald Engine Precompiled Header

#define IMGUI_DEFINE_MATH_OPERATORS
#define GLFW_EXPOSE_NATIVE_WIN32

#define WIN32_LEAN_AND_MEAN 
# pragma warning(push)
# pragma warning(disable: 4668)
# include <WinSock2.h>  //Prevent windows.h from including winsock.h
# include <windows.h>
# pragma warning(pop)

#include <memory>
#include <vector>
#include <string>
#include <array>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <fstream>
#include <atomic>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#pragma warning(push, 0)
#include <json.h>
#pragma warning(pop)

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/integer.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef unsigned char byte;

#include "core/common/staticClass.h"
#include "core/common/ref.h"
#include "core/common/rtti.h"

#include "graphics/misc/glError.h"
#include "core/common/assrt.h"

#include "utils/core/log.h"

#define BIT(n)	(1<<(n))
#define NUMOF(x) (uint32_t)(((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x]))))))