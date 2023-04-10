#pragma once
#include "framework/kvulkan.h"
namespace kvs
{
    class LogicDevice;
    class PhysicalDevice;
    class Buffer;
    class Command;

    class Texture{
    public:
        explicit Texture(const char* filename);
        ~Texture();
        unsigned char * pixels{ nullptr };
        int texWidth, texHeight, texChannels;
        uint32_t mipLevelCount;

        void DestroyTexture();
        VkDeviceSize GetDeviceSize();

    private:
        void CreateTextureImage(const char* filename);
    };

    class Image {
    public:
        Image() = default;
        void Createimage(VkDevice& device, VkPhysicalDevice& pDevice, uint32_t width, uint32_t height, uint32_t mipCounts, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags flags);
        void ClearUp(VkDevice& device);
        void CreateImageView(VkDevice& device, VkFormat format, uint32_t mipCounts);
        VkImage m_Image;
        VkImageView m_ImageView;
        VkDeviceMemory m_ImageMemory;

    private:
    };

    class TextureImage {
    public:
        TextureImage(LogicDevice& logic_device);

        void CreateTextureImage(PhysicalDevice& pDevice, Command& command, Texture& texture);
        void TransitionImageLayout(VkCommandBuffer& cmdBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void CopyTextureImage(VkCommandBuffer& cmdBuffer, VkBuffer& srcBuffer, uint32_t width, uint32_t height);
        void CleanUp();
        void CreateImageView(VkFormat format);
        void CreateSampler(PhysicalDevice& physical);
        void CreateMipMaps(VkCommandBuffer& cmdBuffer, Texture& texture);
        Image m_Image;
        VkSampler m_Sampler;
    private:
        VkDevice& m_Device;
        VkQueue& m_GraphicQueue;
        uint32_t m_MipmapLevelCount;
    };

} // namespace kvs
