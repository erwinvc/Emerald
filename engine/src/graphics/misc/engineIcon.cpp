#include "eepch.h"
#include "engineIcon.h"
#include "GLFW/glfw3.h"
#include "graphics/textures/texture.h"

namespace emerald::icon {
	void loadIcon(void* window) {
		TextureDesc desc;
		desc.readWrite = true;
		Texture texture(desc, 32, 32, icon32_map, NUMOF(icon32_map), TextureDataType::FILE);

		GLFWimage icon[1] = {};
		icon[0].width = 32;
		icon[0].height = 32;
		icon[0].pixels = (byte*)texture.getBuffer().data();

		glfwSetWindowIcon((GLFWwindow*)window, 1, icon);
	}
}