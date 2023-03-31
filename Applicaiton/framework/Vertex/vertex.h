#pragma once
#include "framework/kvulkan.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "framework/Buffer/buffer.h"
namespace kvs
{
    class LogicDevice;
    class PhysicalDevice;
    class Command;

    struct VertexInfo
    {
        glm::vec2 Pos;
        glm::vec3 Color;
    };
    
    using VertexIndexInfo = uint16_t;

    class Vertex {
    public:
        Vertex(std::vector<VertexInfo>& vertices, std::vector<VertexIndexInfo>& indices) : m_vertexs(vertices), m_vertexIndices(indices){

        }

        auto& GetVertexs() {
            return m_vertexs;
        }

        VkVertexInputBindingDescription GetBindingDescription() {
            VkVertexInputBindingDescription des{};
            des.binding = 0;
            des.stride = sizeof(VertexInfo);
            des.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return des;
        }

        auto GetAttributeDescription() {
            std::vector<VkVertexInputAttributeDescription> attr(2);
            attr[0].binding = 0;
            attr[0].location = 0;
            attr[0].format = VK_FORMAT_R32G32_SFLOAT;
            attr[0].offset = offsetof(VertexInfo, Pos);

            attr[1].binding = 0;
            attr[1].location = 1;
            attr[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attr[1].offset = offsetof(VertexInfo, Color);

            return attr;
        }

        std::vector<VertexInfo> m_vertexs;
        std::vector<VertexIndexInfo> m_vertexIndices;
    private:
    };

    class VertexBuffer{
    public:
        VertexBuffer(LogicDevice& logicDevice, PhysicalDevice& physicalDevice, Vertex& vertex);

        void AllocateVertexBuffer(Command& command, VkQueue& queue);
        void FreeVertexBuffer();

        void AllocateIndexBuffer(Command& command, VkQueue& queue);
        void FreeIndexBuffer();

        void CopyBuffer(Buffer& m_StagingBuffer, Buffer& dstBuffer, Command& command, VkDeviceSize size, VkQueue& queue);

        Vertex& m_vertex;
        Buffer m_VertexBuffer;
        Buffer m_IndexBuffer;
    private:
        VkDevice& m_device;
        VkPhysicalDevice& m_pDevice;
    };

} // namespace kvs
