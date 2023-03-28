#include "ImageView/imageview.h"
#include "LogicDevice/logicdevice.h"
#include "SwapChain/swapchain.h"

namespace kvs {
	ImageView::ImageView(LogicDevice& device, SwapChain& swapChain) : m_device(device.GetLogicDevice()), m_swapChain(swapChain)
	{
		auto size = swapChain.m_images.size();
		m_imageViews.resize(size);
	}

	void ImageView::Create()
	{
		auto size = m_imageViews.size();
		for (size_t i = 0; i < size; i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_swapChain.m_images[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_swapChain.m_format.format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			if (vkCreateImageView(m_device, &createInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image view");
			}
		}
	}
	
	void ImageView::Destroy()
	{
		for (auto& imageView : m_imageViews) {
			vkDestroyImageView(m_device, imageView, nullptr);
		}
	}
}

