#include <cstdint>
#include <limits>
#include <algorithm>
#include "SwapChain/swapchain.h"
#include "LogicDevice/logicdevice.h"

#include "WindowSurface/windowsurface.h"

#include "ImageView/imageview.h"
#include "GraphicPipeline/graphicpipeline.h"
#include "framework/Image/depth.h"
#include "framework/Image/msaa.h"
namespace kvs
{
	SwapChain::SwapChain(LogicDevice& device, PhysicalDevice& pDevice, WindowSurface& surface) : 
		m_device(device.GetLogicDevice()), 
		m_surface(surface.GetSurface()), 
		m_indices(pDevice.m_hasFindQueueFamily), 
		m_pdevice(pDevice)
	{
		auto& window = surface.GetWindow();
		m_window = window.GetWindow();
	}
	void SwapChain::Create()
	{
		m_detail = m_pdevice.GetSwapChainSupportDetail();
		glfwGetWindowSize(m_window, &m_width, &m_height);
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;

		auto capability = m_detail.m_capabilities;
		uint32_t imageCount = capability.minImageCount + 1;
		auto maxCount = capability.maxImageCount;
		if (maxCount > 0 && imageCount > maxCount) {
			imageCount = maxCount;
		}
		createInfo.minImageCount = imageCount;

		auto format = ChooseSwapSurfaceFormat();
		m_format = format;
		createInfo.imageFormat = format.format;
		createInfo.imageColorSpace = format.colorSpace;
		auto extent = ChooseSwapExtent();
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		
		std::vector<uint32_t> indices{m_indices.graphicsFamily.value(), m_indices.presentFamily.value()};
		if (indices[0] == indices[1]) {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = indices.size();
			createInfo.pQueueFamilyIndices = indices.data();
		}

		createInfo.preTransform = capability.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = ChooseSwapPresentMode();
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create Swap Chain");
		}

		vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
		m_images.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_images.data());
		
	}

	void SwapChain::Destroy()
	{
		vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
	}

	void SwapChain::CleanUpSwapChain(ImageView& image_view, GraphicPipeline& pipeline)
	{
		pipeline.DestroyFrameBuffer();
		image_view.Destroy();
		Destroy();
	}

	void SwapChain::RecreateSwapChain(ImageView& image_view, GraphicPipeline& pipeline)
	{
		int tempWidth = 0, tempHeight = 0;
		glfwGetFramebufferSize(m_window, &tempWidth, &tempHeight);
		while (tempWidth == 0 || tempHeight == 0) {
			glfwGetFramebufferSize(m_window, &tempWidth, &tempHeight);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_device);

		CleanUpSwapChain(image_view, pipeline);

		Create();
		image_view.Create();

		//Depth depth;
		//depth.CreateDepthResource();
		pipeline.CreateFrameBuffer(image_view.m_imageViews, pipeline.RequestVkRect2D(), Depth{}, Msaa{});
	}

	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat()
	{
		for (auto& format : m_detail.m_formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return format;
			}
		}
		return m_detail.m_formats[0];
	}
	VkPresentModeKHR SwapChain::ChooseSwapPresentMode()
	{
		bool isMailBox = false;
		bool isFifo = false;
		for (auto& mode : m_detail.m_presentModes) {
			if (mode == VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR) {
				isFifo = true;
			}
			else if (mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR) {
				isMailBox = true;
			}
		}
		if (isMailBox) {
			return VK_PRESENT_MODE_MAILBOX_KHR;
		}
		if (isFifo) {
			return VK_PRESENT_MODE_FIFO_KHR;
		}
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}

	VkExtent2D SwapChain::ChooseSwapExtent()
	{
		auto& temp = m_detail.m_capabilities;
		if (temp.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
			return temp.currentExtent;
		}
		else 
		{
			VkExtent2D extent{static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) };

			extent.width = std::clamp(extent.width, temp.minImageExtent.width, temp.maxImageExtent.width);
			extent.height = std::clamp(extent.height, temp.minImageExtent.height, temp.maxImageExtent.height);
			return extent;
		}
	}



} // namespace kvs
