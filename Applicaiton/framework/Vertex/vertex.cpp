#include "vertex.h"
#include "LogicDevice/logicdevice.h"
#include "PhysicalDevice/physicaldevice.h"
#include "CommandBuffer/commandbuffer.h"
namespace kvs{

	VertexBuffer::VertexBuffer(LogicDevice& logicDevice, PhysicalDevice& physicalDevice, Vertex& vertex) :
		m_device(logicDevice.GetLogicDevice()), 
		m_pDevice(physicalDevice.GetPhysicalDevice()),
		m_vertex(vertex)
	{
		
	}

	void VertexBuffer::AllocateVertexBuffer(Command& command, VkQueue& queue)
	{
		VkDeviceSize size = sizeof(m_vertex.m_vertexs[0]) * m_vertex.m_vertexs.size();
		Buffer stagingBuffer;
		stagingBuffer.CreateBuffer(m_device, m_pDevice, size, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		void* data;
		if (vkMapMemory(m_device, stagingBuffer.GetMemory(), 0, size, 0, &data) != VK_SUCCESS) {
			throw std::runtime_error("failed to map memory");
		}
		memcpy(data, m_vertex.m_vertexs.data(), (size_t)size);
		vkUnmapMemory(m_device, stagingBuffer.GetMemory());

		m_VertexBuffer.CreateBuffer(m_device, m_pDevice, size, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		CopyBuffer(stagingBuffer, m_VertexBuffer, command, size, queue);

		stagingBuffer.FreeBufferAndMemory(m_device);
	}

	void VertexBuffer::FreeVertexBuffer()
	{
		m_VertexBuffer.FreeBufferAndMemory(m_device);

	}

	void VertexBuffer::AllocateIndexBuffer(Command& command, VkQueue& queue)
	{
		VkDeviceSize size = sizeof(m_vertex.m_vertexIndices[0]) * m_vertex.m_vertexIndices.size();

		Buffer stagingBuffer;
		stagingBuffer.CreateBuffer(m_device, m_pDevice, size, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(m_device, stagingBuffer.GetMemory(), 0, size, 0, &data);
		memcpy(data, m_vertex.m_vertexIndices.data(), size);
		vkUnmapMemory(m_device, stagingBuffer.GetMemory());

		m_IndexBuffer.CreateBuffer(m_device, m_pDevice, size, VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		CopyBuffer(stagingBuffer, m_IndexBuffer, command, size, queue);

		stagingBuffer.FreeBufferAndMemory(m_device);
	}

	void VertexBuffer::FreeIndexBuffer()
	{
		m_IndexBuffer.FreeBufferAndMemory(m_device);
	}

	void VertexBuffer::CopyBuffer(Buffer& m_StagingBuffer, Buffer& dstBuffer, Command& command, VkDeviceSize size, VkQueue& queue)
	{
		VkCommandBufferAllocateInfo allInfo{};
		allInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allInfo.commandPool = command.m_pool;
		allInfo.commandBufferCount = 1;
		allInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkCommandBuffer cmdBuffer;
		vkAllocateCommandBuffers(m_device, &allInfo, &cmdBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(cmdBuffer, &beginInfo);

		VkBufferCopy bufferCopy{};
		bufferCopy.size = size;
		vkCmdCopyBuffer(cmdBuffer, m_StagingBuffer.GetBuffer(), dstBuffer.GetBuffer(), 1, &bufferCopy);

		vkEndCommandBuffer(cmdBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vkFreeCommandBuffers(m_device, command.m_pool, 1, &cmdBuffer);
	}

}