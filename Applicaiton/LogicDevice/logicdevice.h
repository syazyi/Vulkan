#pragma once
#include "framework/kvulkan.h"
namespace kvs
{
    class PhysicalDevice;
    class LogicDevice{
    public:
        explicit LogicDevice(PhysicalDevice& physicalDevice);
        ~LogicDevice();

        inline VkDevice& GetLogicDevice() {
            return m_Device;
        }
    public:
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
    private:
        VkDevice m_Device;
        VkPhysicalDevice& m_PhysicalDevice;

    };
} // namespace kvs
