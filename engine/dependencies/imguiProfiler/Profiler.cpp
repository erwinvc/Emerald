#include "Profiler.h"

/*
 * This code is adapted from an ImGui-based C++ profiling library originally found on GitHub.
 * Unfortunately, the author's name and the original repository URL are no longer available.
 * All credit for the initial implementation goes to the unknown creator.
 * If you are the author or know who they are, please contact me so I can update the attribution.
 */

#if WITH_PROFILING

namespace ImGuiProfiler {
	CPUProfiler gCPUProfiler;
	GPUProfiler gOpenGLProfiler;
}

#endif
