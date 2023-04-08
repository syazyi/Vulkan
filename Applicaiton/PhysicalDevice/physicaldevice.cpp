#ifndef NDEBUG
#include <iostream>
#endif // NDEBUG

#include <map>
#include <set>
#include "PhysicalDevice/physicaldevice.h"

namespace kvs
{

    PhysicalDevice::PhysicalDevice(KInstance& kInstance, WindowSurface& windowSurface) : m_instance(kInstance.GetInstance()), m_surface(windowSurface.GetSurface())
    {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);
        if (physicalDeviceCount == 0) {
            throw std::runtime_error("failed to enumerate physical device, your device don't have graphic cards");
        }
        std::vector<VkPhysicalDevice> PhysicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, PhysicalDevices.data());

        std::multimap<uint32_t, VkPhysicalDevice*> deviceScore;
        for (auto& device : PhysicalDevices) {
            uint32_t score = EvaluateDeviceScore(device);
            deviceScore.insert(std::make_pair(score, &device));
        }

        if (deviceScore.rbegin()->first > 0) {
            m_device = *deviceScore.rbegin()->second;
        }

        if (!isPhysicalDeviceSuitable() || m_device == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable device");
        }
    }

    PhysicalDevice::~PhysicalDevice()
    {

    }

    void PhysicalDevice::AddPhysicalDeviceExtensionSupport(const char* extension)
    {
        m_deviceExtensionsSupport.push_back(extension);
    }

    void PhysicalDevice::AddPhysicalDeviceExtensionSupport(const std::vector<const char*>& extensions)
    {
        m_deviceExtensionsSupport = extensions;
    }

    uint32_t PhysicalDevice::EvaluateDeviceScore(const VkPhysicalDevice& device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

#ifndef NDEBUG
        DebugPrintPhysicalDeviceInfo(deviceProperties, deviceFeatures);
#endif // NDEBUG
        uint32_t score = 0;
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 100;
        }

        score += deviceProperties.limits.maxImageDimension2D;

        if (!deviceFeatures.geometryShader) {
            return 0;
        }
        return score;
    }

    SwapChainSupportDetail PhysicalDevice::GetSwapChainSupportDetail()
    {
        SwapChainSupportDetail detail{};
        
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device, m_surface, &detail.m_capabilities);

        uint32_t formatsCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_device, m_surface, &formatsCount, nullptr);
        if (formatsCount != 0) {
            detail.m_formats.resize(formatsCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(m_device, m_surface, &formatsCount, detail.m_formats.data());
        }

        uint32_t presentModesCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_device, m_surface, &presentModesCount, nullptr);
        if (presentModesCount != 0) {
            detail.m_presentModes.resize(presentModesCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(m_device, m_surface, &presentModesCount, detail.m_presentModes.data());
        }

        return detail;
    }

    VkFormat PhysicalDevice::GetDepthFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (auto& format : candidates) {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties(m_device, format, &properties);

            if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("not find suitable format");
    }

    void PhysicalDevice::DebugPrintPhysicalDeviceInfo(const VkPhysicalDeviceProperties& pro, const VkPhysicalDeviceFeatures& fea)
    {
#ifndef NDEBUG
        static uint8_t index = 0;
        index++;
        std::cout << "Device" << index << ": \n" << "Properties: \n";
#endif // NDEBUG
    }

    QueueFamilyIndices PhysicalDevice::FindQueueFamilies()
    {
        QueueFamilyIndices indices;
        uint32_t queueFamilyPropertyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queueFamilyPropertyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queueFamilyPropertyCount, queueFamilyProperties.data());

        uint32_t i = 0;
        for (auto& qfp : queueFamilyProperties) {
            if (qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            if (vkGetPhysicalDeviceSurfaceSupportKHR(m_device, i, m_surface, &presentSupport) != VK_SUCCESS) {
                throw std::runtime_error("failed to get physical device surface support");
            }
            if (presentSupport) {
                indices.presentFamily = i;
            }
            i++;
        }

        m_hasFindQueueFamily = indices;
        return indices;
    }

    bool PhysicalDevice::isPhysicalDeviceSuitable()
    {
        auto queueFamily = FindQueueFamilies();

        bool extensionSupport = HasPhysicalDeviceExtensions();

        SwapChainSupportDetail detail = GetSwapChainSupportDetail();
        if (queueFamily.hasValue() && extensionSupport && !detail.m_formats.empty() && !detail.m_presentModes.empty()) {
            return true; 
        }
        
        return false;
    }

    bool PhysicalDevice::HasPhysicalDeviceExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(m_device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(m_device, nullptr, &extensionCount, extensions.data());

        std::set<std::string_view> requiredCheckedExtensionNames(m_deviceExtensionsSupport.begin(), m_deviceExtensionsSupport.end());
        for (auto& extension : extensions) {
            requiredCheckedExtensionNames.erase(extension.extensionName);
        }
        if (requiredCheckedExtensionNames.empty()) {
            return true;
        }
        return false;
    }

} // namespace kvs
