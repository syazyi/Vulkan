#include "windowsurface.h"

namespace kvs {
	WindowSurface::WindowSurface(KInstance& instance, Window& window) : m_instance(instance.GetInstance()), m_window(window)
	{
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.flags = 0;
		createInfo.hwnd = m_window.GetWin32WindowHandle();
		createInfo.hinstance = m_window.GetWin32WindowInstace();

		if (vkCreateWin32SurfaceKHR(m_instance, &createInfo, nullptr, &m_surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create Window Surface");
		}
	}

	WindowSurface::~WindowSurface()
	{
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	}

	void WindowSurface::GetWindowSurfaceCreateFunction()
	{
		//Technically this is a WSI extension function, 
		//but it is so commonly used that the standard Vulkan loader includes it, 
		//so unlike other extensions you don't need to explicitly load it.
	}


}

