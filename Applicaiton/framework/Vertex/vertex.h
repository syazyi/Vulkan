#pragma once
#include "framework/kvulkan.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
namespace kvs
{
    class LogicDevice;
    class PhysicalDevice;

    struct VertexInfo
    {
        glm::vec2 Pos;
        glm::vec3 Color;
    };
    
    class Vertex {
    public:
        Vertex(std::vector<VertexInfo>& vertices) : m_vertexs(vertices) {

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
    private:
    };

    class VertexBuffer {
    public:
        VertexBuffer(LogicDevice& logicDevice, PhysicalDevice& physicalDevice, Vertex& vertex);

        void AllocateVertexBuffer();
        void FreeVertexBuffer();
        VkBuffer m_vertexBuffer;
        Vertex& m_vertex;
    private:
        VkDevice& m_device;
        VkPhysicalDevice& m_pDevice;

        VkDeviceMemory m_deviceMemory;

    };

} // namespace kvs
