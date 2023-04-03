#pragma once
#include "framework/kvulkan.h"
#include "glm/mat4x4.hpp"

namespace kvs
{
    class Buffer;
    class LogicDevice;
    class SwapChain;

    struct VertexTransformMat
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    class Uniform {
    public:
        Uniform(LogicDevice& logic_device);

        void CreateUniformBuffer(VkPhysicalDevice& physical_device);

        void UpdateUniform(SwapChain& swap_chain);

        void ClearUpUniform();
    public:
        std::vector<Buffer> m_Buffers;
    private:
        VkDevice& m_Device;
        std::vector<void*> m_MapData;
    };

} // namespace kvs
