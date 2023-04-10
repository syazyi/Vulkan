#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "framework/Uniform/uniform.h"
#include "framework/Buffer/buffer.h"
#include "LogicDevice/logicdevice.h"
#include "SwapChain/swapchain.h"

namespace kvs
{

    Uniform::Uniform(LogicDevice& logic_device) : m_Device(logic_device.GetLogicDevice())
    {
        m_Buffers.resize(frame_in_flight);
        m_MapData.resize(frame_in_flight);
    }

    void Uniform::CreateUniformBuffer(VkPhysicalDevice& physical_device)
    {
        auto size = sizeof(VertexTransformMat);
        for (int i = 0; i < frame_in_flight; i++) {
            m_Buffers[i].CreateBuffer(m_Device, physical_device, size, 
                VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkMapMemory(m_Device, m_Buffers[i].GetMemory(), 0, size, 0, &m_MapData[i]);
        }
    }

    void Uniform::UpdateUniform(SwapChain& swap_chain)
    {
        static auto start_time = std::chrono::high_resolution_clock::now();
        auto current_time = std::chrono::high_resolution_clock::now();
        auto duration_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

        VertexTransformMat mat{};
        mat.model = glm::rotate(glm::mat4(1.0f), duration_time * glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));
        //mat.model = glm::mat4(1.0f);
        mat.view = glm::lookAt(glm::vec3(2.f, 2.f, 2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto extent = swap_chain.ChooseSwapExtent();
        mat.proj = glm::perspective(glm::radians(45.f), static_cast<float>(extent.width / extent.height), 0.1f, 100.f);
        mat.proj[1][1] *= -1;

        memcpy(m_MapData[currentFrame], &mat, sizeof(mat));
    }

    void Uniform::CleanUpUniform()
    {
        for (auto& buffer : m_Buffers) {
            vkUnmapMemory(m_Device, buffer.GetMemory());
            buffer.FreeBufferAndMemory(m_Device);
        }
        for (auto& data : m_MapData) {
            data = nullptr;
        }
    }

} // namespace kvs
