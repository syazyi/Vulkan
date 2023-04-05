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

        void DestroyTexture();
        VkDeviceSize GetDeviceSize();

    private:
        void CreateTextureImage(const char* filename);
    };

    class Image {
    public:
        Image() = default;
        void Createimage(VkDevice& device, VkPhysicalDevice& pDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags flags);
        void ClearUp(VkDevice& device);
        VkImage m_Image;
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
    private:
        VkDevice& m_Device;
        Image m_Image;
        VkQueue& m_GraphicQueue;
    };

} // namespace kvs
