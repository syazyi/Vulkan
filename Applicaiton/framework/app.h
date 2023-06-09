#pragma once 
#include "framework/kvulkan.h"
namespace kvs
{
    class LogicDevice;
    class SwapChain;
    class Command;
    class GraphicPipeline;
    class ImageView;
    class VertexBuffer;
    class Uniform;
    class Descriptor;

    class App{
    public:
        explicit App(LogicDevice& device, Command& command);
        void CreateSyncObject();
        void DestroySyncObject();
        void DrawFrame(GraphicPipeline& drawPass, SwapChain& swapchain, ImageView& imageView, VertexBuffer& vertex_buffer, Uniform& uniform, Descriptor& descriptor);

    private:
        
    private:
        VkDevice m_device;
        Command& m_command;

        VkQueue& m_graphicQueue;
        VkQueue& m_presentQueue;

        std::vector<VkSemaphore> m_imageToRender;
        std::vector<VkSemaphore> m_renderToPresent;
        std::vector<VkFence> m_oneFrame;


    };

} // namespace kvs
