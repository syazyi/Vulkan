#pragma once 
#include <optional>
#include "framework/kvulkan.h"
#include "framework/Utility.h"
#include "vulkanInstance/include/vkInstance.h"
#include "WindowSurface/windowsurface.h"
namespace kvs
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool hasValue() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    class PhysicalDevice{
    public:

        explicit PhysicalDevice(KInstance& kInstance, WindowSurface& windowSurface);
        ~PhysicalDevice();

        static void AddPhysicalDeviceExtensionSupport(const char* extension);
        static void AddPhysicalDeviceExtensionSupport(const std::vector<const char*>& extensions);

        inline VkPhysicalDevice& GetPhysicalDevice() {
            return m_device;
        }
        uint32_t EvaluateDeviceScore(const VkPhysicalDevice& device);

        SwapChainSupportDetail GetSwapChainSupportDetail();
        VkFormat GetDepthFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkSampleCountFlagBits GetMaxUsableSampleCount();

    public:
        inline static std::vector<const char*> m_deviceExtensionsSupport{};
        
        QueueFamilyIndices m_hasFindQueueFamily{};
    private:
        VkInstance& m_instance;
        VkPhysicalDevice m_device{ VK_NULL_HANDLE };
        

        QueueFamilyIndices FindQueueFamilies();

        VkSurfaceKHR& m_surface;

        void DebugPrintPhysicalDeviceInfo(const VkPhysicalDeviceProperties& pro, const VkPhysicalDeviceFeatures& fea);
        bool isPhysicalDeviceSuitable();

        bool HasPhysicalDeviceExtensions();
    };
    
} // namespace kvs
