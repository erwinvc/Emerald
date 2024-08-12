#include "eepch.h"
#include "engineIcon.h"
#include "GLFW/glfw3.h"
#include "texture.h"


namespace emerald::icon {
	void loadIcon(GLFWwindow* window) {
		TextureDesc desc;
		desc.m_readWrite = true;
		Texture texture(desc, 32, 32, icon32_map, NUMOF(icon32_map), TextureDataType::FILE);

		GLFWimage icon[1] = {};
		icon[0].width = 32;
		icon[0].height = 32;
		icon[0].pixels = (byte*)texture.getBuffer().data();

		glfwSetWindowIcon(window, 1, icon);
	}
}