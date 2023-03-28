#pragma once
#include "framework/kvulkan.h"
#include "framework/Utility.h"
namespace kvs {
	class KInstance {
	public:
		KInstance();
		~KInstance();

		inline VkInstance& GetInstance() {
			return instance;
		}

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
		void SetDebugCallback();
	private:
		bool CheckInstanceVaildationLayerSupport();

		std::vector<const char*> GetRequiredExtensions();

		VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pMessenger
			);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);

		void SetVkDebugUtilsMessengerCreateInfoEXT(VkDebugUtilsMessengerCreateInfoEXT& createInfo);


		VkDebugUtilsMessengerEXT callback;
		VkInstance instance;
	public:
		inline static const std::vector<const char*> vaildLayerNames{ "VK_LAYER_KHRONOS_validation" };
	};
}