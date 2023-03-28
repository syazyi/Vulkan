#include "framework/kvulkan.h"
#include "PhysicalDevice/physicaldevice.h"
namespace kvs
{
    class LogicDevice;
    class SwapChain;
    class GraphicPipeline;
    class Command {
    public:
        Command(LogicDevice& device);

        void CreateCommand(QueueFamilyIndices indices);
        void RecordDrawCommand(uint32_t index, GraphicPipeline& drawPass, SwapChain& swapchain);
        void DestroyCommand();

    private:
        void CreateCommandPool(QueueFamilyIndices& indices);
        void DestroyCommandPool();

        void AllocateCommandBuffer();

    private:
        VkDevice& m_device;

        VkCommandPool m_pool;
    public:
        std::vector<VkCommandBuffer> m_drawCommandBuffer;
    };
    
} // namespace kvs
