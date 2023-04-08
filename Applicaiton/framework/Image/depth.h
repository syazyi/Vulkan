#pragma once 
#include "framework/kvulkan.h"

namespace kvs
{
    class LogicDevice;
    class SwapChain;
    class PhysicalDevice;
    class Command;

    class Depth {
    public:
        Depth() = default;

        void CreateDepthResource(LogicDevice& device, SwapChain& swapchain, PhysicalDevice& physical_device, Command& command);

        void DestroyDepthResource(VkDevice& device);
        VkImage m_DepthImage;
        VkDeviceMemory m_DepthImageMemory;
        VkImageView m_DepthImageView;
        VkFormat depthFormat;
    };


} // namespace kvs
