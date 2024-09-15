#pragma once

namespace emerald {
	namespace GLFW {
		struct GLFWConfiguration {
			int majorVersion = 4;
			int minorVersion = 6;
			int depthBits = 24;
			bool resizable = true;
			bool visible = false;
		};

		bool initialize(const GLFWConfiguration& config);
	}
}