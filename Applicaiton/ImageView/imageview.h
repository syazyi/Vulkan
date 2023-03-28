#pragma once
#include "framework/kvulkan.h"
namespace kvs
{
    class LogicDevice;
    class SwapChain;

    class ImageView{
    public:
        explicit ImageView(LogicDevice& device, SwapChain& swapChain);
        ~ImageView(){};

        void Create();
        void Destroy();
    public:
        std::vector<VkImageView> m_imageViews;
    private:
        SwapChain& m_swapChain;
        VkDevice& m_device;
    };

    
} // namespace kvs
