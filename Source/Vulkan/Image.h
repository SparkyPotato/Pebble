#pragma once

#include "Instance.h"

class Image;

class ImageView
{
public:
	ImageView() = default;
	ImageView(const Image& image, VkFormat format, VkImageViewType viewType, VkComponentMapping mapping,
		VkImageSubresourceRange range, VkImageViewCreateFlags flags = 0);
	~ImageView();

	ImageView(const ImageView& other) = delete;
	ImageView& operator=(const ImageView& other) = delete;

	ImageView(ImageView&& other);
	ImageView& operator=(ImageView&& other);

	VkImageView GetHandle() const { return m_View; }

private:
	friend class Swapchain;

	ImageView(VkImage vkImage, VkFormat format, VkImageViewType viewType, VkComponentMapping mapping,
		VkImageSubresourceRange range, VkImageViewCreateFlags flags = 0);

	VkImageView m_View = VK_NULL_HANDLE;
};

class Image
{
public:
	Image() = default;
	Image(VkImageType type, VkFormat format, glm::u32vec3 size, u32 mipLevels, u32 layers,
		VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageLayout layout, VmaMemoryUsage memUsage,
		VkImageCreateFlags flags = 0);
	~Image();

	Image(const Image& other) = delete;
	Image& operator=(const Image& other) = delete;

	Image(Image&& other);
	Image& operator=(Image&& other);

	VkImage GetHandle() const { return m_Image; }
	VmaAllocation GetMemory() const { return m_Memory; }

private:
	VkImage m_Image = VK_NULL_HANDLE;
	VmaAllocation m_Memory = VK_NULL_HANDLE;
};
