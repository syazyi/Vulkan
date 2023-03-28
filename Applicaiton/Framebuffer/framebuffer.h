#include "framework/kvulkan.h"

namespace kvs
{
    class SwapChain;
    class ImageView;
    class Framebuffer {
    public:
        Framebuffer();

        void Create(std::vector<ImageView>& imageViews);
        void Destroy();

    private:
        SwapChain& m_swapChain;
        std::vector<VkFramebuffer> m_framebuffers;
    };


} // namespace kvs
