#pragma once 

#include "framework/kvulkan.h"
#include <map>

namespace kvs
{
    class MemoryManager{
    public:
        MemoryManager() = default;

         VkDeviceMemory& FindAndPush(VkDevice& device, VkPhysicalDevice& pDevice, VkBuffer& buffer, VkMemoryPropertyFlags propertyFlags, VkDeviceSize size){
            
            VkMemoryRequirements requirements;
            vkGetBufferMemoryRequirements(device, buffer, &requirements);

            VkPhysicalDeviceMemoryProperties properties{};
            vkGetPhysicalDeviceMemoryProperties(pDevice, &properties);

            uint32_t i = 0;
            for(; i < properties.memoryTypeCount; i++){
                if(requirements.memoryTypeBits & (1 << i) && properties.memoryTypes[i].propertyFlags & (propertyFlags)){
                    break;
                }
            }

            auto it = m_memories.find(i);
            if(it == m_memories.end()){
                VkMemoryAllocateInfo allInfo{};
                allInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allInfo.allocationSize = size;
                allInfo.memoryTypeIndex = i;
                vkAllocateMemory(device, &allInfo, nullptr, &m_memories[i]);
            }

            return m_memories[i];
        }


    private:
        std::map<uint32_t, VkDeviceMemory> m_memories;

        //存储每个内存中目前的偏移量。
        //当buffer绑定时，使用当前偏移量。buffer自己的size，和其在内存中的位置（偏移）由buffer自己存储。
        //这个位置可以利用这里存储的目前的的偏移量直接拷贝得到。
        //这样就能有目前内存的已经使用的量。各个buffer存储其在这个内存中的位置。
        std::map<VkDeviceMemory&, uint32_t> m_MemoryOffset;
    };



} // namespace kvs
