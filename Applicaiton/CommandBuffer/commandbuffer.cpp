#include "CommandBuffer/commandbuffer.h"
#include "LogicDevice/logicdevice.h"
#include "SwapChain/swapchain.h"
#include "GraphicPipeline/graphicpipeline.h"
namespace kvs
{
    Command::Command(LogicDevice& device) : m_device(device.GetLogicDevice()){
        m_drawCommandBuffer.resize(frame_in_flight);
    }

    void Command::CreateCommand(QueueFamilyIndices indices)
    {
        CreateCommandPool(indices);

        AllocateCommandBuffer();
    }

    void Command::DestroyCommand()
    {
        DestroyCommandPool();
    }

    void Command::CreateCommandPool(QueueFamilyIndices& indices)
    {
        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = indices.graphicsFamily.value();
        createInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(m_device, &createInfo, nullptr, &m_pool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create Command Pool");
        }
    }

    void Command::DestroyCommandPool()
    {
        vkDestroyCommandPool(m_device, m_pool, nullptr);
    }

    void Command::AllocateCommandBuffer()
    {
        VkCommandBufferAllocateInfo cbainfo{};
        cbainfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cbainfo.commandPool = m_pool;
        cbainfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cbainfo.commandBufferCount = frame_in_flight;

        if (vkAllocateCommandBuffers(m_device, &cbainfo, m_drawCommandBuffer.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffer!");
        }
    }

    void Command::RecordDrawCommand(uint32_t index, GraphicPipeline& drawPass, SwapChain& swapchain, VertexBuffer& vertex_buffer)
    {
        auto drawCommandBuffer = m_drawCommandBuffer[currentFrame];
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(drawCommandBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to Begin Command buffer");
        }


        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = drawPass.GetRenderPass();
        renderPassBeginInfo.framebuffer = drawPass.GetFramebuffers()[index];
        renderPassBeginInfo.renderArea.offset = {0, 0};

        auto extent = swapchain.ChooseSwapExtent();
        renderPassBeginInfo.renderArea.extent = extent;

        VkClearValue value{0.0f, 0.0f, 0.0f, 1.0f};
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &value;
        vkCmdBeginRenderPass(drawCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawPass.GetPipeline());

        VkViewport viewport{};
        viewport.width = 1600.f;
        viewport.height = 900.f;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(drawCommandBuffer, 0, 1, &viewport);

        VkRect2D rect2d{};
        rect2d.extent = extent;
        rect2d.offset = { 0, 0 };
        vkCmdSetScissor(drawCommandBuffer, 0, 1, &rect2d);

        VkBuffer buffers[] = { vertex_buffer.m_VertexBuffer.GetBuffer()};
        VkDeviceSize deviceSize[] = { 0 };
        vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, buffers, deviceSize);

        vkCmdDraw(drawCommandBuffer, vertex_buffer.m_vertex.m_vertexs.size(), 1, 0, 0);

        vkCmdEndRenderPass(drawCommandBuffer);

        if (vkEndCommandBuffer(drawCommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to end command buffer");
        }
    }



} // namespace kvs
