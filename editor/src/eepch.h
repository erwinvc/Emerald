#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#define GLFW_EXPOSE_NATIVE_WIN32

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include <memory>
#include <vector>
#include <string>
#include <array>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <fstream>
#include <atomic>

#pragma warning(push, 0)
#include <json.h>
#pragma warning(pop)

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/integer.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "ref.h"
#include "rtti.h"

#include "assrt.h"

#include "util/log.h"

#define NUMOF(x) (uint32_t)((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))