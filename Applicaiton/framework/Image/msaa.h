#pragma once 
#include "framework/kvulkan.h"

namespace kvs
{
    class LogicDevice;
    class SwapChain;
    class PhysicalDevice;
    class Command;

    class Msaa {
    public:
        Msaa() = default;

        void CreateMsaaResource(LogicDevice& device, SwapChain& swapchain, PhysicalDevice& physical_device, Command& command);

        void DestroyMsaaResource(VkDevice& device);
        VkImage m_MsaaImage;
        VkDeviceMemory m_MsaaImageMemory;
        VkImageView m_MsaaImageView;
        VkSampleCountFlagBits m_SampleCount;
        //VkFormat msaaFormat;
    };


} // namespace kvs
