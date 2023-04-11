#include "depth.h"
#include "SwapChain/swapchain.h"
#include "LogicDevice/logicdevice.h"
#include "PhysicalDevice/physicaldevice.h"
#include "CommandBuffer/commandbuffer.h"
namespace kvs
{
    void Depth::CreateDepthResource(LogicDevice& device, SwapChain& swapchain, PhysicalDevice& physical_device, Command& command)
    {
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        depthFormat = physical_device.GetDepthFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT }, VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        imageCreateInfo.format = depthFormat;

        auto& extent = swapchain.ChooseSwapExtent();
        imageCreateInfo.extent.width = extent.width;
        imageCreateInfo.extent.height = extent.height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = physical_device.GetMaxUsableSampleCount();
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        vkCreateImage(device.GetLogicDevice(), &imageCreateInfo, nullptr, &m_DepthImage);

        VkMemoryRequirements requires{};
        vkGetImageMemoryRequirements(device.GetLogicDevice(), m_DepthImage, &requires);

        VkMemoryAllocateInfo allInfo{};
        allInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allInfo.allocationSize = requires.size;

        VkPhysicalDeviceMemoryProperties memoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(physical_device.GetPhysicalDevice(), &memoryProperties);

        uint32_t index = 0;
        for (; index < memoryProperties.memoryTypeCount; index++) {
            if (requires.memoryTypeBits & (1 << index) && (memoryProperties.memoryTypes[index].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
                break;
            }
        }
        allInfo.memoryTypeIndex = index;

        vkAllocateMemory(device.GetLogicDevice(), &allInfo, nullptr, &m_DepthImageMemory);
        vkBindImageMemory(device.GetLogicDevice(), m_DepthImage, m_DepthImageMemory, 0);


        VkImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = m_DepthImage;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = depthFormat;
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;
        viewCreateInfo.subresourceRange.levelCount = 1;

        vkCreateImageView(device.GetLogicDevice(), &viewCreateInfo, nullptr, &m_DepthImageView);
        
    }

    void Depth::DestroyDepthResource(VkDevice& device)
    {
        vkDestroyImageView(device, m_DepthImageView, nullptr);
        vkFreeMemory(device, m_DepthImageMemory, nullptr);
        vkDestroyImage(device, m_DepthImage, nullptr);
    }

} // namespace kvs
