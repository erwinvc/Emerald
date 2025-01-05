#include "eepch.h"
#include "engineIcon.h"
#include "GLFW/glfw3.h"
#include "graphics/textures/texture.h"
#include "engine/assets/loaders/textureLoader.h"

namespace emerald::icon {
	void loadIcon(void* window) {
		TextureDesc desc;
		desc.readWrite = true;
		TextureLoader loader(desc, icon32_map, NUMOF(icon32_map), false);
		Ref<Texture> tex = loader.load();

		GLFWimage icon[1] = {};
		icon[0].width = 32;
		icon[0].height = 32;
		icon[0].pixels = (byte*)tex->getBuffer().data();

		glfwSetWindowIcon((GLFWwindow*)window, 1, icon);
	}
}