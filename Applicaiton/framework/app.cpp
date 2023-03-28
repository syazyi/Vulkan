#include "framework/app.h"
#include "LogicDevice/logicdevice.h"
#include "SwapChain/swapchain.h"
#include "CommandBuffer/commandbuffer.h"
#include "ImageView/imageview.h"
#include "window/include/window.h"
namespace kvs
{
    App::App(LogicDevice& device, Command& command) : m_device(device.GetLogicDevice()), m_command(command), 
        m_graphicQueue(device.m_GraphicsQueue),
        m_presentQueue(device.m_PresentQueue){
        m_imageToRender.resize(frame_in_flight);
        m_renderToPresent.resize(frame_in_flight);
        m_oneFrame.resize(frame_in_flight);
    }


    void App::CreateSyncObject()
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < frame_in_flight; i++) {
            if (vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_imageToRender[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_renderToPresent[i]) != VK_SUCCESS ||
                vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_oneFrame[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create SyncObject");
            }
        }

    }

    void App::DestroySyncObject()
    {
        for (size_t i = 0; i < frame_in_flight; i++) {
            vkDestroySemaphore(m_device, m_renderToPresent[i], nullptr);
            vkDestroySemaphore(m_device, m_imageToRender[i], nullptr);
            vkDestroyFence(m_device, m_oneFrame[i], nullptr);
        }
    }

    void App::DrawFrame(GraphicPipeline& drawPass, SwapChain& swapchain, ImageView& imageView)
    {
        vkWaitForFences(m_device, 1, &m_oneFrame[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        auto result_AcquireNextImage = vkAcquireNextImageKHR(m_device, swapchain.GetSwapChain(), UINT64_MAX, m_imageToRender[currentFrame], VK_NULL_HANDLE, &imageIndex);
        if (result_AcquireNextImage == VkResult::VK_ERROR_OUT_OF_DATE_KHR) {
            swapchain.RecreateSwapChain(imageView, drawPass);
            return;
        }else if (result_AcquireNextImage != VK_SUCCESS && result_AcquireNextImage != VkResult::VK_SUBOPTIMAL_KHR){
            throw std::runtime_error("failed to acquire next image");
        }

        vkResetFences(m_device, 1, &m_oneFrame[currentFrame]);

        vkResetCommandBuffer(m_command.m_drawCommandBuffer[currentFrame], 0);
        m_command.RecordDrawCommand(imageIndex, drawPass, swapchain);

        VkSubmitInfo subInfo{};
        subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_imageToRender[currentFrame]};
        VkPipelineStageFlags stages[] = {VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        subInfo.waitSemaphoreCount = 1;
        subInfo.pWaitSemaphores = waitSemaphores;
        subInfo.pWaitDstStageMask = stages;
        subInfo.commandBufferCount = 1;
        subInfo.pCommandBuffers = &m_command.m_drawCommandBuffer[currentFrame];

        VkSemaphore signalSemaphores[] = { m_renderToPresent[currentFrame]};
        subInfo.signalSemaphoreCount = 1;
        subInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_graphicQueue, 1, &subInfo, m_oneFrame[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit command");
        }

        
        VkPresentInfoKHR preInfo{};
        preInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        preInfo.waitSemaphoreCount = 1;
        preInfo.pWaitSemaphores = signalSemaphores;
        preInfo.swapchainCount = 1;

        VkSwapchainKHR swapChains[] = { swapchain.GetSwapChain()};
        preInfo.pSwapchains = swapChains;
        preInfo.pImageIndices = &imageIndex;
        preInfo.pResults = nullptr;

        auto result_present = vkQueuePresentKHR(m_presentQueue, &preInfo);
        if (result_present == VK_ERROR_OUT_OF_DATE_KHR || result_present == VK_SUBOPTIMAL_KHR || Window::framebufferResized) {
            swapchain.RecreateSwapChain(imageView, drawPass);
            Window::framebufferResized = false;
        }
        else if (result_present != VK_SUCCESS){
            throw std::runtime_error("failed to queue present");
        }

        currentFrame = (currentFrame + 1) % frame_in_flight;
    }

} // namespace kvs
