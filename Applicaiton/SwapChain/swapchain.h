#pragma once
#include "framework/kvulkan.h"
#include "PhysicalDevice/physicaldevice.h"

namespace kvs
{
    class LogicDevice;
    class WindowSurface;
    class ImageView;
    class GraphicPipeline;

    class SwapChain
    {
    public:
        explicit SwapChain(LogicDevice& device, PhysicalDevice& pDevice, WindowSurface& surface);
        ~SwapChain() {};

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat();
        VkPresentModeKHR ChooseSwapPresentMode();
        VkExtent2D ChooseSwapExtent();

        void Create();
        void Destroy();

        VkSwapchainKHR& GetSwapChain() {
            return m_swapChain;
        }

        void CleanUpSwapChain(ImageView& image_view, GraphicPipeline& pipeline);

        void RecreateSwapChain(ImageView& image_view, GraphicPipeline& pipeline);

    public:
        std::vector<VkImage> m_images;
        VkSurfaceFormatKHR m_format;
    private:
        VkSwapchainKHR m_swapChain;
        VkDevice& m_device;
        VkSurfaceKHR& m_surface;
        GLFWwindow* m_window;
        PhysicalDevice& m_pdevice;

        SwapChainSupportDetail m_detail;

        QueueFamilyIndices m_indices{};
        int m_width;
        int m_height;
    };
} // namespace kvs
