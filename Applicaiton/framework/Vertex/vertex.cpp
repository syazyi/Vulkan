#include "vertex.h"
#include "LogicDevice/logicdevice.h"
#include "PhysicalDevice/physicaldevice.h"
namespace kvs{

	VertexBuffer::VertexBuffer(LogicDevice& logicDevice, PhysicalDevice& physicalDevice, Vertex& vertex) :
		m_device(logicDevice.GetLogicDevice()), 
		m_pDevice(physicalDevice.GetPhysicalDevice()),
		m_vertex(vertex)
	{
		
	}

	void VertexBuffer::AllocateVertexBuffer()
	{
		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = sizeof(m_vertex.m_vertexs[0]) * m_vertex.m_vertexs.size();
		bufferCreateInfo.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_device, &bufferCreateInfo, nullptr, &m_vertexBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer");
		}

		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(m_device, m_vertexBuffer, &requirements);

		VkPhysicalDeviceMemoryProperties memory_properties{};
		vkGetPhysicalDeviceMemoryProperties(m_pDevice, &memory_properties);
		uint32_t i = 0;
		for (; i < memory_properties.memoryTypeCount; i++) {
			if ((requirements.memoryTypeBits & (1 << i)) && 
				(memory_properties.memoryTypes[i].propertyFlags & 
				(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))) {
				break;
			}
		}

		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.memoryTypeIndex = i;
		allocateInfo.allocationSize = requirements.size;

		if (vkAllocateMemory(m_device, &allocateInfo, nullptr, &m_deviceMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate memory");
		}

		if (vkBindBufferMemory(m_device, m_vertexBuffer, m_deviceMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("failed to Bind buffer memory");
		}

		void* data;
		if (vkMapMemory(m_device, m_deviceMemory, 0, bufferCreateInfo.size, 0, &data) != VK_SUCCESS) {
			throw std::runtime_error("failed to map memory");
		}
		memcpy(data, m_vertex.m_vertexs.data(), (size_t)bufferCreateInfo.size);
		vkUnmapMemory(m_device, m_deviceMemory);
	}

	void VertexBuffer::FreeVertexBuffer()
	{
		vkFreeMemory(m_device, m_deviceMemory, nullptr);
		vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);

	}



}