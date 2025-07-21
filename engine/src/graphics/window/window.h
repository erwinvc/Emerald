#pragma once
#include "GLWindowCallbacks.h"
#include "utils/math/color.h"

namespace emerald {
	struct WindowSettings {
		std::string m_title;
		uint32_t m_width, m_height;
	};

	class Window : public RefCounted {
	private:
		std::string m_title;
		uint32_t m_width, m_height;
		GLWindowCallbacks m_callbacks;

		GLFWwindow* m_window = nullptr;
		bool m_vSync = false;
		bool m_isFocussed = true;
		bool m_isMinimized = false;

		void onFocusEvent(uint32_t focus) { m_isFocussed = focus; }

	public:
		Window(std::string title, int width, int height);
		~Window();

		GLWindowCallbacks& getCallbacks() { return m_callbacks; }
		uint32_t getWidth() const { return m_width; }
		uint32_t getHeight() const { return m_height; }
		float getAspect() const { return (float)(m_width) / m_height; }
		bool isFocussed() const { return m_isFocussed; }

		void setVSync(bool toggle);
		void setTitle(const std::string& title);
		const std::string& getTitle() { return m_title; }


		void swapBuffers();
		void pollEvents();
		bool shouldClose();
		void makeContextCurrent();
		void show();

		void clearColor(const Color& col);
		void clear();

		bool getVSync() const { return m_vSync; }
		bool isMinimized() const  { return m_isMinimized; }
		bool isMaximized() const;
		void minimize();
		void maximize();
		void restore();

		void setWidth(uint32_t width);
		void setHeight(uint32_t height);
		void setLimits(uint32_t minWidth, uint32_t minHeight, uint32_t maxWidth, uint32_t maxHeight);

		//inline void setIcon(const Icon& icon) { GL(glfwSetWindowIcon(GetHandle(), 1, icon.GetImage())); }
		void onResize(uint32_t width, uint32_t height);

		GLFWwindow* handle() const { return m_window; }
	};
}