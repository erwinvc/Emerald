#pragma once
#include "OpenGLProfiler.h"

/*
 * This code is adapted from an ImGui-based C++ profiling library originally found on GitHub.
 * Unfortunately, the author's name and the original repository URL are no longer available.
 * All credit for the initial implementation goes to the unknown creator.
 * If you are the author or know who they are, please contact me so I can update the attribution.
 */

#ifndef WITH_PROFILING
#define WITH_PROFILING 1
#endif

#if WITH_PROFILING

#define PROFILE_REGISTER_THREAD(...) ImGuiProfiler::gCPUProfiler.RegisterThread(__VA_ARGS__)

#define PROFILE_FRAME() ImGuiProfiler::gCPUProfiler.Tick(); ImGuiProfiler::gOpenGLProfiler.Tick()

#define PROFILE_SCOPE(...)		ImGuiProfiler::CPUProfileScope MACRO_CONCAT(profiler, __COUNTER__)(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define PROFILE_GPU_SCOPE(...)	ImGuiProfiler::CPUProfileScope MACRO_CONCAT(profiler, __COUNTER__)(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__); \
								ImGuiProfiler::OpenGLProfileScope MACRO_CONCAT(profiler, __COUNTER__)(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__) 

#define PROFILE_BEGIN(...)		ImGuiProfiler::gCPUProfiler.BeginEvent(__VA_ARGS__)
#define PROFILE_GPU_BEGIN(...)	ImGuiProfiler::gCPUProfiler.BeginEvent(__VA_ARGS__); ImGuiProfiler::gOpenGLProfiler.BeginEvent(__VA_ARGS__)

#define PROFILE_END()			ImGuiProfiler::gCPUProfiler.EndEvent()
#define PROFILE_GPU_END()		ImGuiProfiler::gCPUProfiler.EndEvent(); ImGuiProfiler::gOpenGLProfiler.EndEvent()

#define PROFILE_INITIALIZE() ImGuiProfiler::gCPUProfiler.Initialize("CPU", 8, 1024); ImGuiProfiler::gOpenGLProfiler.Initialize("GPU", 8, 1024);

#define PROFILE_DISABLE() ImGuiProfiler::gCPUProfiler.SetPaused(true); ImGuiProfiler::gOpenGLProfiler.SetPaused(true);
#else

#define PROFILE_REGISTER_THREAD(...)
#define PROFILE_FRAME()

#define PROFILE_SCOPE(...)
#define PROFILE_GPU_SCOPE(...)

#define PROFILE_BEGIN(...)
#define PROFILE_GPU_BEGIN(...)

#define PROFILE_END()
#define PROFILE_GPU_END()

#define PROFILE_INITIALIZE()
#define PROFILE_DISABLE()

#endif

namespace ImGuiProfiler {
	void DrawProfilerHUD();

	// Global CPU Profiler
	extern class CPUProfiler gCPUProfiler;
	extern class CPUProfiler gGPUProfiler;
	extern class GPUProfiler gOpenGLProfiler;

	// Helper RAII-style structure to push and pop a CPU sample region
	struct CPUProfileScope {
		CPUProfileScope(const char* pFunctionName, const char* pFilePath, uint32 lineNumber, const char* pName) {
			gCPUProfiler.BeginEvent(pName, pFilePath, lineNumber);
		}

		CPUProfileScope(const char* pFunctionName, const char* pFilePath, uint32 lineNumber) {
			gCPUProfiler.BeginEvent(pFunctionName, pFilePath, lineNumber);
		}

		~CPUProfileScope() {
			gCPUProfiler.EndEvent();
		}

		CPUProfileScope(const CPUProfileScope&) = delete;
		CPUProfileScope& operator=(const CPUProfileScope&) = delete;
	};

	struct OpenGLProfileScope {
		OpenGLProfileScope(const char* pFunctionName, const char* pFilePath, uint32 lineNumber, const char* pName) {
			gOpenGLProfiler.BeginEvent(pName, pFilePath, lineNumber);
		}

		OpenGLProfileScope(const char* pFunctionName, const char* pFilePath, uint32 lineNumber) {
			gOpenGLProfiler.BeginEvent(pFunctionName, pFilePath, lineNumber);
		}

		~OpenGLProfileScope() {
			gOpenGLProfiler.EndEvent();
		}

		OpenGLProfileScope(const OpenGLProfileScope&) = delete;
		OpenGLProfileScope& operator=(const OpenGLProfileScope&) = delete;
	};
}