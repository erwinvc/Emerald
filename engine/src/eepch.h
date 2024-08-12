#pragma once //Emerald Engine Precompiled Header

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

#include "assrt.h"

#include "util/log.h"

#define NUMOF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))