#include "framework/Buffer/buffer.h"
#include "LogicDevice/logicdevice.h"
#include "PhysicalDevice/physicaldevice.h"
namespace kvs
{
    uint32_t Buffer::FindMemoryIndex(VkPhysicalDevice& pDevice, VkMemoryRequirements requires, VkMemoryPropertyFlags flags) {
        VkPhysicalDeviceMemoryProperties propertyies{};
        vkGetPhysicalDeviceMemoryProperties(pDevice, &propertyies);

        uint32_t index = 0;
        for (; index < propertyies.memoryTypeCount; index++) {
            if (requires.memoryTypeBits & (1 << index) && propertyies.memoryTypes[index].propertyFlags & (flags)) {
                break;
            }
        }
        return index;
    }

    

    void Buffer::CreateBuffer(VkDevice& device, VkPhysicalDevice& pDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags)
    {
        VkBufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = size;
        createInfo.usage = usage;
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &createInfo, nullptr, &m_buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }
        VkMemoryRequirements requirements{};
        vkGetBufferMemoryRequirements(device, m_buffer, &requirements);

        VkMemoryAllocateInfo allInfo{};
        allInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allInfo.memoryTypeIndex = FindMemoryIndex(pDevice, requirements, flags);
        allInfo.allocationSize = requirements.size;
        
        if (vkAllocateMemory(device, &allInfo, nullptr, &m_deviceMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate memory!");
        }

        vkBindBufferMemory(device, m_buffer, m_deviceMemory, 0);
    }

    void Buffer::FreeBufferAndMemory(VkDevice& device)
    {
        vkDestroyBuffer(device, m_buffer, nullptr);
        vkFreeMemory(device, m_deviceMemory, nullptr);
    }

} // namespace kvs
