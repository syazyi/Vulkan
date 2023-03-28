#include "GraphicPipeline/graphicpipeline.h"

#include "LogicDevice/logicdevice.h"
#include "SwapChain/swapchain.h"

#include "framework/Vertex/vertex.h"
namespace kvs
{
    

    
    GraphicPipeline::GraphicPipeline(LogicDevice& device, SwapChain& swapchain, const std::vector<uint32_t>& vertex_shader, const std::vector<uint32_t>& fragment_shader) :
        m_device(device.GetLogicDevice()), m_swapChain(swapchain), m_vertShader(vertex_shader), m_fragShader(fragment_shader)
    {

    }

    void GraphicPipeline::CreatePipeline(VertexBuffer& vertex_buffer)
    {
        //create render pass
        CreateRenderPass();
        //create pipeline layout
        VkPipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        if (vkCreatePipelineLayout(m_device, &layoutCreateInfo, nullptr, &m_layout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }

        //wrapper shader code
        VkShaderModule vert_shader_module;
        CreateShaderMoudle(vert_shader_module, m_vertShader);
        VkShaderModule frag_shader_module;
        CreateShaderMoudle(frag_shader_module, m_fragShader);

        //set Modifiable pipeline stage
        VkPipelineShaderStageCreateInfo vert_shader_stage_createInfo{};
        vert_shader_stage_createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_createInfo.module = vert_shader_module;
        vert_shader_stage_createInfo.pName = "main";
        vert_shader_stage_createInfo.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo frag_shader_stage_createInfo{};
        frag_shader_stage_createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_createInfo.module = frag_shader_module;
        frag_shader_stage_createInfo.pName = "main";
        frag_shader_stage_createInfo.pSpecializationInfo = nullptr;

        VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {vert_shader_stage_createInfo, frag_shader_stage_createInfo};

        //set dynamic state
        std::vector<VkDynamicState> dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        auto dynamicState = EnableDynamicState(dynamicStates);

        auto bindDes = vertex_buffer.m_vertex.GetBindingDescription();
        auto attrDes = vertex_buffer.m_vertex.GetAttributeDescription();
        //fixable pipeline stage
        auto vertexInput = RequestVertexInputStateCreateInfo(&bindDes, 1, attrDes.data(), attrDes.size());
        auto inputAssmbly = RequestInputAssemblyStateCreateInfo();

        auto viewportNeed = RequestVkViewport();
        auto rect2d = RequestVkRect2D();
        auto viewport = RequestViewportStateCreateInfo(viewportNeed, rect2d);

        auto rasterization = RequestRasterizationStateCreateInfo();
        auto multiSample = RequestMultisampleStateCreateInfo();
        auto depthStencil = RequestDepthStencilStateCreateInfo();

        auto colorBlendAttachment = RequestColorAttachment();
        auto colorBlend = RequestColorBlendStateCreateInfo(colorBlendAttachment);

        VkGraphicsPipelineCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = 2;
        createInfo.pStages = shaderStageCreateInfos;
        createInfo.pVertexInputState = &vertexInput;
        createInfo.pInputAssemblyState = &inputAssmbly;
        createInfo.pTessellationState = nullptr;
        createInfo.pViewportState = &viewport;
        createInfo.pRasterizationState = &rasterization;
        createInfo.pMultisampleState = &multiSample;
        createInfo.pDepthStencilState = &depthStencil;
        createInfo.pColorBlendState = &colorBlend;
        createInfo.pDynamicState = &dynamicState;
        createInfo.layout = m_layout;
        createInfo.renderPass = m_renderpass;
        createInfo.subpass = 0;
        createInfo.basePipelineHandle = VK_NULL_HANDLE;
        createInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline");
        }


        DestroyShaderMoudle(vert_shader_module);
        DestroyShaderMoudle(frag_shader_module);
    }

    void GraphicPipeline::DestroyPipeline()
    {
        
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, m_layout, nullptr);
        DestroyRenderPass();

    }

    void GraphicPipeline::CreateRenderPass()
    {
        VkAttachmentDescription attachmentDescription{};
        attachmentDescription.format = m_swapChain.m_format.format;
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


        VkAttachmentReference attachReference{};
        attachReference.attachment = 0;
        attachReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


        VkSubpassDescription subpassDescroption{};
        subpassDescroption.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescroption.colorAttachmentCount = 1;
        subpassDescroption.pColorAttachments = &attachReference;

        VkRenderPassCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &attachmentDescription;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpassDescroption;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;
        

        if (vkCreateRenderPass(m_device, &createInfo, nullptr, &m_renderpass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass");
        }
    }

    void GraphicPipeline::DestroyRenderPass()
    {
        vkDestroyRenderPass(m_device, m_renderpass, nullptr);
    }

    void GraphicPipeline::CreateFrameBuffer(std::vector<VkImageView>& imageViews, VkRect2D rect)
    {
        //createFramebuffers
        auto imageViewSize = imageViews.size();
        m_framebuffers.resize(imageViewSize);
        for (size_t i = 0; i < imageViewSize; i++) {
            std::vector<VkImageView> imageViewsNeed = {
                imageViews[i]
            };

            VkFramebufferCreateInfo framebufferCIF{};
            framebufferCIF.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

            framebufferCIF.renderPass = m_renderpass;
            framebufferCIF.attachmentCount = 1;
            framebufferCIF.pAttachments = imageViewsNeed.data();
            framebufferCIF.width = rect.extent.width;
            framebufferCIF.height = rect.extent.height;
            framebufferCIF.layers = 1;

            if (vkCreateFramebuffer(m_device, &framebufferCIF, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void GraphicPipeline::DestroyFrameBuffer()
    {
        for (auto& framebuffer : m_framebuffers) {
            vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        }
    }

    void GraphicPipeline::CreateShaderMoudle(VkShaderModule& shader_module, std::vector<uint32_t>& code)
    {
        VkShaderModuleCreateInfo vertCreateInfo{};
        vertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertCreateInfo.codeSize = code.size() * 4;
        vertCreateInfo.pCode = code.data();
            if (vkCreateShaderModule(m_device, &vertCreateInfo, nullptr, &shader_module) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shader moudle");
        }
    }

    void GraphicPipeline::DestroyShaderMoudle(VkShaderModule& shader_module)
    {
        vkDestroyShaderModule(m_device, shader_module, nullptr);
    }

    VkPipelineVertexInputStateCreateInfo GraphicPipeline::RequestVertexInputStateCreateInfo(VkVertexInputBindingDescription* bind, uint32_t bindCount, VkVertexInputAttributeDescription* attr, uint32_t attrCount){
        VkPipelineVertexInputStateCreateInfo VertexInputCreateInfo{};
        VertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VertexInputCreateInfo.vertexBindingDescriptionCount = bindCount;
        VertexInputCreateInfo.pVertexBindingDescriptions = bind;
        VertexInputCreateInfo.vertexAttributeDescriptionCount = attrCount;
        VertexInputCreateInfo.pVertexAttributeDescriptions = attr;
        return VertexInputCreateInfo;
    }

    VkPipelineInputAssemblyStateCreateInfo GraphicPipeline::RequestInputAssemblyStateCreateInfo()
    {
        VkPipelineInputAssemblyStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        createInfo.primitiveRestartEnable = VK_FALSE;
        return createInfo;
    }

    VkViewport GraphicPipeline::RequestVkViewport()
    {
        VkViewport viewport{};
        viewport.width = 1600;
        viewport.height = 900;
        viewport.x = 0;
        viewport.y = 0;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        return viewport;
    }

    VkRect2D GraphicPipeline::RequestVkRect2D()
    {
        VkRect2D scissior{};
        scissior.extent = m_swapChain.ChooseSwapExtent();
        scissior.offset = { 0, 0 };
        return scissior;
    }

    VkPipelineViewportStateCreateInfo GraphicPipeline::RequestViewportStateCreateInfo(VkViewport& viewport, VkRect2D scissior)
    {
        VkPipelineViewportStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        createInfo.viewportCount = 1;
        createInfo.pViewports = &viewport;
        createInfo.scissorCount = 1;
        createInfo.pScissors = &scissior;

        return createInfo;
    }

    VkPipelineRasterizationStateCreateInfo GraphicPipeline::RequestRasterizationStateCreateInfo()
    {
        VkPipelineRasterizationStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        createInfo.depthClampEnable = VK_FALSE;
        createInfo.rasterizerDiscardEnable = VK_FALSE;
        createInfo.polygonMode = VK_POLYGON_MODE_FILL;
        createInfo.cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
        createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        createInfo.depthBiasEnable = VK_FALSE;
        createInfo.lineWidth = 1.0f;

        return createInfo;
    }

    VkPipelineMultisampleStateCreateInfo GraphicPipeline::RequestMultisampleStateCreateInfo()
    {
        VkPipelineMultisampleStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        createInfo.sampleShadingEnable = VK_FALSE;
        return createInfo;
    }

    VkPipelineDepthStencilStateCreateInfo GraphicPipeline::RequestDepthStencilStateCreateInfo()
    {
        VkPipelineDepthStencilStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        createInfo.depthTestEnable = VK_FALSE;
        createInfo.stencilTestEnable = VK_FALSE;
        return createInfo;
    }

    VkPipelineColorBlendAttachmentState GraphicPipeline::RequestColorAttachment()
    {
        VkPipelineColorBlendAttachmentState colorAttachment{};
        colorAttachment.colorWriteMask = VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT |
            VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT |
            VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT |
            VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
        colorAttachment.blendEnable = VK_FALSE;
        colorAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        return colorAttachment;
    }

    VkPipelineColorBlendStateCreateInfo GraphicPipeline::RequestColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState& colorAttachment)
    {
        VkPipelineColorBlendStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        createInfo.logicOpEnable = VK_FALSE;
        createInfo.logicOp = VK_LOGIC_OP_COPY;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &colorAttachment;
        createInfo.blendConstants[0] = 0.0f;
        createInfo.blendConstants[1] = 0.0f;
        createInfo.blendConstants[2] = 0.0f;
        createInfo.blendConstants[3] = 0.0f;
        return createInfo;
    }

    VkPipelineDynamicStateCreateInfo GraphicPipeline::EnableDynamicState(std::vector<VkDynamicState>& dynamicStates)
    {
        VkPipelineDynamicStateCreateInfo dynamicCreateInfo{};
        dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicCreateInfo.dynamicStateCount = dynamicStates.size();
        dynamicCreateInfo.pDynamicStates = dynamicStates.data();
        return dynamicCreateInfo;
    }



} // namespace kvs
