#pragma once 

#include "framework/kvulkan.h"

namespace kvs
{
    class LogicDevice;
    class PhysicalDevice;

    class Buffer {
    public:
        Buffer() = default;
        virtual ~Buffer() {};

        uint32_t FindMemoryIndex(VkPhysicalDevice& pDevice, VkMemoryRequirements requires, VkMemoryPropertyFlags flags);
        void CreateBuffer(VkDevice& device, VkPhysicalDevice& pDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags);
        void FreeBufferAndMemory(VkDevice& device);
        VkBuffer& GetBuffer() {
            return m_buffer;
        }
        VkDeviceMemory& GetMemory() {
            return m_deviceMemory;
        }
    protected:
        VkBuffer m_buffer;
        VkDeviceMemory m_deviceMemory;
    };


} // namespace kvs
