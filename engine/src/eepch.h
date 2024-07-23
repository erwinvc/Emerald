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

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "util/log.h"