#pragma once
#include "OpenGLProfiler.h"

#ifndef WITH_PROFILING
#define WITH_PROFILING 1
#endif

#if WITH_PROFILING

/*
	General
*/

// Usage:
//		PROFILE_REGISTER_THREAD(const char* pName)
//		PROFILE_REGISTER_THREAD()
#define PROFILE_REGISTER_LOGIC_THREAD(...) ImGuiProfiler::gCPUProfiler.RegisterThread(__VA_ARGS__)
#define PROFILE_REGISTER_RENDER_THREAD(...) ImGuiProfiler::gGPUProfiler.RegisterThread("Render")

/// Usage:
//		PROFILE_FRAME()
#define PROFILE_LOGIC_FRAME() ImGuiProfiler::gCPUProfiler.Tick()
#define PROFILE_RENDER_FRAME() ImGuiProfiler::gGPUProfiler.Tick()
#define PROFILE_OPENGL_FRAME() ImGuiProfiler::gOpenGLProfiler.Tick()

/*
	CPU Profiling
*/

// Usage:
//		PROFILE_CPU_SCOPE(const char* pName)
//		PROFILE_CPU_SCOPE()
#define PROFILE_LOGIC_SCOPE(...)							ImGuiProfiler::CPUProfileScope MACRO_CONCAT(profiler, __COUNTER__)(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define PROFILE_RENDER_SCOPE(...)							ImGuiProfiler::GPUProfileScope MACRO_CONCAT(profiler, __COUNTER__)(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define PROFILE_OPENGL_SCOPE(...)							ImGuiProfiler::OpenGLProfileScope MACRO_CONCAT(profiler, __COUNTER__)(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

// Usage:
//		PROFILE_CPU_BEGIN(const char* pName)
//		PROFILE_CPU_BEGIN()
#define PROFILE_LOGIC_BEGIN(...)							ImGuiProfiler::gCPUProfiler.BeginEvent(__VA_ARGS__)
#define PROFILE_RENDER_BEGIN(...)							ImGuiProfiler::gGPUProfiler.BeginEvent(__VA_ARGS__); PROFILE_OPENGL_BEGIN(__VA_ARGS__)
#define PROFILE_OPENGL_BEGIN(...)							ImGuiProfiler::gOpenGLProfiler.BeginEvent(__VA_ARGS__)
// Usage:
//		PROFILE_CPU_END()
#define PROFILE_LOGIC_END()									ImGuiProfiler::gCPUProfiler.EndEvent()
#define PROFILE_RENDER_END()								ImGuiProfiler::gGPUProfiler.EndEvent(); PROFILE_OPENGL_END();
#define PROFILE_OPENGL_END(...)								ImGuiProfiler::gOpenGLProfiler.EndEvent()

#define PROFILE_INITIALIZE() ImGuiProfiler::gCPUProfiler.Initialize("CPU", 8, 1024 * 4); ImGuiProfiler::gGPUProfiler.Initialize("GPU", 8, 1024 * 4); ImGuiProfiler::gOpenGLProfiler.Initialize("GPU", 8, 1024 * 4);

#define PROFILE_DISABLE() ImGuiProfiler::gCPUProfiler.SetPaused(true); ImGuiProfiler::gGPUProfiler.SetPaused(true); ImGuiProfiler::gOpenGLProfiler.SetPaused(true);
#else

#define PROFILE_REGISTER_LOGIC_THREAD(...)
#define PROFILE_REGISTER_RENDER_THREAD(...)
#define PROFILE_LOGIC_FRAME()
#define PROFILE_RENDER_FRAME()
#define PROFILE_OPENGL_FRAME()

#define PROFILE_LOGIC_SCOPE(...)
#define PROFILE_RENDER_SCOPE(...)
#define PROFILE_OPENGL_SCOPE(...)

#define PROFILE_LOGIC_BEGIN(...)
#define PROFILE_RENDER_BEGIN(...)
#define PROFILE_OPENGL_BEGIN(...)

#define PROFILE_LOGIC_END()
#define PROFILE_RENDER_END()
#define PROFILE_OPENGL_END()

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

	struct GPUProfileScope {
		GPUProfileScope(const char* pFunctionName, const char* pFilePath, uint32 lineNumber, const char* pName) {
			gGPUProfiler.BeginEvent(pName, pFilePath, lineNumber);
		}

		GPUProfileScope(const char* pFunctionName, const char* pFilePath, uint32 lineNumber) {
			gGPUProfiler.BeginEvent(pFunctionName, pFilePath, lineNumber);
		}

		~GPUProfileScope() {
			gGPUProfiler.EndEvent();
		}

		GPUProfileScope(const GPUProfileScope&) = delete;
		GPUProfileScope& operator=(const GPUProfileScope&) = delete;
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