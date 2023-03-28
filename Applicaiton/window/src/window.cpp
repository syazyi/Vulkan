#include "window/include/window.h"

namespace kvs {
	Window::Window(const int width, const int height) : Width(width), Height(height) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        
        window = glfwCreateWindow(width, height, "Vulkan Demo", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow * window, int width, int height) {
            auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            app->framebufferResized = true;
        });
    }

    Window::~Window(){
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    HWND Window::GetWin32WindowHandle()
    {
        return glfwGetWin32Window(window);
    }

    HMODULE Window::GetWin32WindowInstace()
    {
        return GetModuleHandle(nullptr);
    }

}
