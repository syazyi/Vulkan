#include <stdexcept>
#include <iostream>
#include "vulkanInstance/include/vkInstance.h"
#include "GLFW/glfw3.h"
namespace kvs {
	KInstance::KInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_VERSION_1_0;
		appInfo.pApplicationName = "Vulkan Demo";
		appInfo.applicationVersion = VK_VERSION_1_0;
		appInfo.apiVersion = VK_API_VERSION_1_3;


		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		if constexpr(bEnableVaildLayer == true){

			bool layerFound = CheckInstanceVaildationLayerSupport();
			if (layerFound == false)
			{
				throw std::runtime_error("Dont found Layer!\n");
			}
			else{
				createInfo.ppEnabledLayerNames = vaildLayerNames.data();
				createInfo.enabledLayerCount = static_cast<uint32_t>(vaildLayerNames.size());
			}
		}
		else{
			createInfo.enabledLayerCount = 0;
		}

		auto glfwExtensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
		createInfo.ppEnabledExtensionNames = glfwExtensions.data();

		if (vkCreateInstance(&createInfo, nullptr, &instance)) {
			throw std::runtime_error("Create Instance Failed");
		}

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()) != VK_SUCCESS) {
			throw std::runtime_error("Get Extensions Error");
		}
		std::cout << "Available Extensions: " << extensionCount << "\n";
		for (const auto& extension : extensions) {
			std::cout << extension.extensionName << "\n";
		}

		if constexpr (bEnableVaildLayer == true) {
			SetDebugCallback();
		}

	}

	KInstance::~KInstance()
	{
		if constexpr (bEnableVaildLayer == true) {
			DestroyDebugUtilsMessengerEXT(instance, callback, nullptr);
		}
		vkDestroyInstance(instance, nullptr);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL KInstance::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{		
		std::cerr << "validation layer: \n" << pCallbackData->pMessage << "\n\n";
		return VK_FALSE;
	}

	bool KInstance::CheckInstanceVaildationLayerSupport()
	{
		uint32_t instanceLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
		std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());

		bool layerFound = false;
		for (const auto& vaildLayerName : vaildLayerNames) {
			for (const auto& instanceLayerProperty : instanceLayerProperties) {
				if (strcmp(instanceLayerProperty.layerName, vaildLayerName) == 0) {
					layerFound = true;
					break;
				}
			}
			if (layerFound == false) {
				return false;
			}
		}
		return true;
	}

	std::vector<const char*> KInstance::GetRequiredExtensions()
	{
		uint32_t extensionsCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionsCount);
		if constexpr (bEnableVaildLayer == true) {
			extensions.push_back("VK_EXT_debug_utils");
		}
		return extensions;
	}

	void KInstance::SetDebugCallback()
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		SetVkDebugUtilsMessengerCreateInfoEXT(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up callback function");
		}
	}

	VkResult KInstance::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func == nullptr) {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
			return func(instance, pCreateInfo, pAllocator, pMessenger);
	}

	void KInstance::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func == nullptr) {
			return;
		}
		func(instance, messenger, pAllocator);
	}

	void KInstance::SetVkDebugUtilsMessengerCreateInfoEXT(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
	}

}
