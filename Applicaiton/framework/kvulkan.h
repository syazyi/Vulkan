#pragma once 
#include "framework/macro.h"

#include "vulkan.hpp"


namespace kvs {
	struct SwapChainSupportDetail
	{
		VkSurfaceCapabilitiesKHR m_capabilities;
		std::vector<VkSurfaceFormatKHR> m_formats;
		std::vector<VkPresentModeKHR> m_presentModes;
	};

	constexpr uint32_t frame_in_flight = 2;
	extern uint32_t currentFrame;
}