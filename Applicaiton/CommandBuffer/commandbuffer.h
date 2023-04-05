#include <functional>
#include "framework/kvulkan.h"
#include "PhysicalDevice/physicaldevice.h"
#include "framework/Vertex/vertex.h"
namespace kvs
{
    class LogicDevice;
    class SwapChain;
    class GraphicPipeline;
    class Descriptor;
    class Command {
    public:
        Command(LogicDevice& device);

        void CreateCommand(QueueFamilyIndices indices);
        void RecordDrawCommand(uint32_t index, GraphicPipeline& drawPass, SwapChain& swapchain, VertexBuffer& vertex_buffer, Descriptor& descriptor);
        void DestroyCommand();

        void BeginEndSingleTimeCommands(LogicDevice& logic_device, std::function<void(void)> func);
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer& cmdBuffer, VkQueue& queue);

        VkCommandPool m_pool;
    private:
        void CreateCommandPool(QueueFamilyIndices& indices);
        void DestroyCommandPool();

        void AllocateCommandBuffer();


    private:
        VkDevice& m_device;

    public:
        std::vector<VkCommandBuffer> m_drawCommandBuffer;
    };
    
} // namespace kvs
