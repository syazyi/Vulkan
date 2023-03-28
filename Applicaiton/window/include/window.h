#pragma once

#include "framework/kvulkan.h"
#include <GLFW/glfw3.h>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif // WIN32
#include <GLFW/glfw3native.h>

namespace kvs {
    class Window{
    public:
        int Width{1600};
        int Height{900};

        Window() = delete;
        Window(const int width, const int height);
        ~Window();

        //be used in Window Surface Create
        HWND GetWin32WindowHandle();
        HMODULE GetWin32WindowInstace();

        inline GLFWwindow* GetWindow() {
            return window;
        }
        inline static bool framebufferResized{ false };
    private:
        GLFWwindow* window;

    };
}