#include "stdafx.h"

void GLCallbackManager::Initialize(Window* window) {
    glfwSetWindowUserPointer(window->GetHandle(), this);

    Timer t;

    glfwSetWindowSizeCallback(window->GetHandle(), [](GLFWwindow* wnd, int width, int height) {
        GLCallbackManager* callbackManager = (GLCallbackManager*)glfwGetWindowUserPointer(wnd);
        for (auto& callback : callbackManager->m_onResizeCallbacks) {
            callback(width, height);
        }
    });

    glfwSetWindowCloseCallback(window->GetHandle(), [](GLFWwindow* wnd) {
        GLCallbackManager* callbackManager = (GLCallbackManager*)glfwGetWindowUserPointer(wnd);
        for (auto& callback : callbackManager->m_onCloseCallbacks) {
            callback();
        }
    });

    glfwSetKeyCallback(window->GetHandle(), [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
        GLCallbackManager* callbackManager = (GLCallbackManager*)glfwGetWindowUserPointer(wnd);
        for (auto& callback : callbackManager->m_onKeyCallbacks) {
            callback(key, scancode, action, mods);
        }
    });

    glfwSetMouseButtonCallback(window->GetHandle(), [](GLFWwindow* wnd, int button, int action, int mods) {
        GLCallbackManager* callbackManager = (GLCallbackManager*)glfwGetWindowUserPointer(wnd);
        for (auto& callback : callbackManager->m_onMouseButtonCallbacks) {
            callback(button, action, mods);
        }
    });

    glfwSetCursorPosCallback(window->GetHandle(), [](GLFWwindow* wnd, double xpos, double ypos) {
        GLCallbackManager* callbackManager = (GLCallbackManager*)glfwGetWindowUserPointer(wnd);
        for (auto& callback : callbackManager->m_onMousePosCallbacks) {
            callback(xpos, ypos);
        }
    });

    glfwSetScrollCallback(window->GetHandle(), [](GLFWwindow* wnd, double xoffset, double yoffset) {
        GLCallbackManager* callbackManager = (GLCallbackManager*)glfwGetWindowUserPointer(wnd);
        for (auto& callback : callbackManager->m_onScrollCallbacks) {
            callback(xoffset, yoffset);
        }
    });

    glfwSetCharCallback(window->GetHandle(), [](GLFWwindow* wnd, uint character) {
        GLCallbackManager* callbackManager = (GLCallbackManager*)glfwGetWindowUserPointer(wnd);
        for (auto& callback : callbackManager->m_onKeyTypedCallbacks) {
            callback(character);
        }
    });
}