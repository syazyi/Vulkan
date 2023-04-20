#include <set>
#include "LogicDevice/logicdevice.h"
#include "PhysicalDevice/physicaldevice.h"
namespace kvs
{
    LogicDevice::LogicDevice(PhysicalDevice& physicalDevice) : m_PhysicalDevice(physicalDevice.GetPhysicalDevice())
    {

        std::vector<VkDeviceQueueCreateInfo> qCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies{ 
            physicalDevice.m_hasFindQueueFamily.graphicsFamily.value(), 
            physicalDevice.m_hasFindQueueFamily.presentFamily.value()
        };
        for (auto& index : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo qCreateInfo{};
            qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

            qCreateInfo.queueFamilyIndex = index;
            qCreateInfo.queueCount = 1;
            float priorities = 1.0f;
            qCreateInfo.pQueuePriorities = &priorities; 

            qCreateInfos.push_back(qCreateInfo);
        }

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = qCreateInfos.size();
        createInfo.pQueueCreateInfos = qCreateInfos.data();

        VkPhysicalDeviceFeatures physicalDeviceFeatures{};
        physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
        physicalDeviceFeatures.sampleRateShading = VK_TRUE;
        createInfo.pEnabledFeatures = &physicalDeviceFeatures;
      
        createInfo.enabledExtensionCount = PhysicalDevice::m_deviceExtensionsSupport.size();
        createInfo.ppEnabledExtensionNames = PhysicalDevice::m_deviceExtensionsSupport.data();

        if constexpr (bEnableVaildLayer == true) {
            createInfo.enabledLayerCount = KInstance::vaildLayerNames.size();
            createInfo.ppEnabledLayerNames = KInstance::vaildLayerNames.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logic device");
        }

        vkGetDeviceQueue(m_Device, physicalDevice.m_hasFindQueueFamily.graphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, physicalDevice.m_hasFindQueueFamily.presentFamily.value(), 0, &m_PresentQueue);
    }
    LogicDevice::~LogicDevice()
    {
        vkDestroyDevice(m_Device, nullptr);
    }
} // namespace kvs
