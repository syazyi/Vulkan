#pragma once

#include "framework/kvulkan.h"

namespace kvs
{
    class LogicDevice;
    class SwapChain;
    class VertexBuffer;

    class GraphicPipeline{
    public:
        GraphicPipeline(LogicDevice& device, SwapChain& swapchain, const std::vector<uint32_t>& vertex_shader, const std::vector<uint32_t>& fragment_shader);
        ~GraphicPipeline(){}

        void CreatePipeline(VertexBuffer& vertex_buffer);
        void DestroyPipeline();

        void CreateRenderPass();
        void DestroyRenderPass();

        VkRenderPass& GetRenderPass() {
            return m_renderpass;
        }

        VkPipeline& GetPipeline() {
            return m_pipeline;
        }

        std::vector<VkFramebuffer>& GetFramebuffers() {
            return m_framebuffers;
        }

        void CreateFrameBuffer(std::vector<VkImageView>& imageViews, VkRect2D rect);
        void DestroyFrameBuffer();
        VkRect2D RequestVkRect2D();

    private:
        void CreateShaderMoudle(VkShaderModule& shader_module, std::vector<uint32_t>& code);
        void DestroyShaderMoudle(VkShaderModule& shader_module);
        VkPipelineVertexInputStateCreateInfo RequestVertexInputStateCreateInfo(VkVertexInputBindingDescription* bind, uint32_t bindCount, VkVertexInputAttributeDescription* attr, uint32_t attrCount);
        VkPipelineInputAssemblyStateCreateInfo RequestInputAssemblyStateCreateInfo();

        VkViewport RequestVkViewport();
        VkPipelineViewportStateCreateInfo RequestViewportStateCreateInfo(VkViewport& viewport, VkRect2D scissior);

        VkPipelineRasterizationStateCreateInfo RequestRasterizationStateCreateInfo();
        VkPipelineMultisampleStateCreateInfo RequestMultisampleStateCreateInfo();
        VkPipelineDepthStencilStateCreateInfo RequestDepthStencilStateCreateInfo();

        VkPipelineColorBlendAttachmentState RequestColorAttachment();
        VkPipelineColorBlendStateCreateInfo RequestColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState& colorAttachment);

        VkPipelineDynamicStateCreateInfo EnableDynamicState(std::vector<VkDynamicState>& states);
    private:

        VkDevice& m_device;
        SwapChain& m_swapChain;
        std::vector<uint32_t> m_vertShader;
        std::vector<uint32_t> m_fragShader;

        //it Only have one render pass now
        VkRenderPass m_renderpass;

        VkPipelineLayout m_layout;

        VkPipeline m_pipeline;

        std::vector<VkFramebuffer> m_framebuffers;
    };


} // namespace kvs
