#include "framework/Image/msaa.h"

#include "SwapChain/swapchain.h"
#include "LogicDevice/logicdevice.h"
#include "PhysicalDevice/physicaldevice.h"
#include "CommandBuffer/commandbuffer.h"

namespace kvs {

	void Msaa::CreateMsaaResource(LogicDevice& device, SwapChain& swapchain, PhysicalDevice& physical_device, Command& command)
	{
		
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = swapchain.m_format.format;

		auto extent = swapchain.ChooseSwapExtent();
		imageInfo.extent.depth = 1;
		imageInfo.extent.width = extent.width;
		imageInfo.extent.height = extent.height;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		m_SampleCount = physical_device.GetMaxUsableSampleCount();
		imageInfo.samples = m_SampleCount;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		vkCreateImage(device.GetLogicDevice(), &imageInfo, nullptr, &m_MsaaImage);

		VkMemoryRequirements require{};
		vkGetImageMemoryRequirements(device.GetLogicDevice(), m_MsaaImage, &require);

		VkPhysicalDeviceMemoryProperties pdMemPro{};
		vkGetPhysicalDeviceMemoryProperties(physical_device.GetPhysicalDevice(), &pdMemPro);

		uint32_t i = 0;
		for (; i < pdMemPro.memoryTypeCount; i++) {
			if ((require.memoryTypeBits & (1 << i)) && (pdMemPro.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
				break;
			}
		}

		VkMemoryAllocateInfo allInfo{};
		allInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allInfo.allocationSize = require.size;
		allInfo.memoryTypeIndex = i;
		vkAllocateMemory(device.GetLogicDevice(), &allInfo, nullptr, &m_MsaaImageMemory);

		vkBindImageMemory(device.GetLogicDevice(), m_MsaaImage, m_MsaaImageMemory, 0);

		VkImageViewCreateInfo ivCreateInfo{};
		ivCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ivCreateInfo.image = m_MsaaImage;
		ivCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivCreateInfo.format = swapchain.m_format.format;
		ivCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ivCreateInfo.subresourceRange.baseArrayLayer = 0;
		ivCreateInfo.subresourceRange.baseMipLevel = 0;
		ivCreateInfo.subresourceRange.layerCount = 1;
		ivCreateInfo.subresourceRange.levelCount = 1;

		vkCreateImageView(device.GetLogicDevice(), 
			&ivCreateInfo, nullptr, &m_MsaaImageView);

	}

	void Msaa::DestroyMsaaResource(VkDevice& device)
	{
		vkDestroyImageView(device, m_MsaaImageView, nullptr);
		vkFreeMemory(device, m_MsaaImageMemory, nullptr);
		vkDestroyImage(device, m_MsaaImage, nullptr);
	}

}