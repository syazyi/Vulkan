#pragma once
#include "framework/kvulkan.h"
#include "window/include/window.h"
#include "vulkanInstance/include/vkInstance.h"

namespace kvs
{
    class WindowSurface {
    public:
        explicit WindowSurface(KInstance& instance, Window& window);
        ~WindowSurface();

        inline VkSurfaceKHR& GetSurface() {
            return m_surface;
        }

        inline Window& GetWindow() {
            return m_window;
        }
    private:
        void GetWindowSurfaceCreateFunction();
    private:
        VkInstance& m_instance;
        Window& m_window;
        VkSurfaceKHR m_surface;
    };
} // namespace kvs

