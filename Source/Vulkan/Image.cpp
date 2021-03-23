#include "PCH.h"

#include "Image.h"

ImageView::ImageView(VkImage vkImage, VkFormat format, VkImageViewType viewType, VkComponentMapping mapping,
	VkImageSubresourceRange range, VkImageViewCreateFlags flags)
{
	VkImageViewCreateInfo info{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = flags,
		.image = vkImage,
		.viewType = viewType,
		.format = format,
		.components = mapping,
		.subresourceRange = range };

	VkCall(vkCreateImageView(Instance::Device(), &info, nullptr, &m_View));
}

ImageView::ImageView(const Image& image, VkFormat format, VkImageViewType viewType, VkComponentMapping mapping,
	VkImageSubresourceRange range, VkImageViewCreateFlags flags)
{
	ImageView(image.GetHandle(), format, viewType, mapping, range, flags);
}

ImageView::~ImageView() { vkDestroyImageView(Instance::Device(), m_View, nullptr); }

ImageView::ImageView(ImageView&& other)
{
	m_View = other.m_View;
	other.m_View = VK_NULL_HANDLE;
}

ImageView& ImageView::operator=(ImageView&& other)
{
	this->~ImageView();

	m_View = other.m_View;
	other.m_View = VK_NULL_HANDLE;

	return *this;
}

Image::Image(VkImageType type, VkFormat format, glm::u32vec3 size, u32 mipLevels, u32 layers,
	VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageLayout layout, VmaMemoryUsage memUsage,
	VkImageCreateFlags flags)
{
	u32 index = Instance::GraphicsIndex();

	VkImageCreateInfo info{ .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.flags = flags,
		.imageType = type,
		.format = format,
		.extent = { size.x, size.y, size.z },
		.mipLevels = mipLevels,
		.arrayLayers = layers,
		.samples = samples,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &index,
		.initialLayout = layout };

	VmaAllocationCreateInfo allocInfo{ .usage = memUsage };

	VkCall(vmaCreateImage(Instance::Allocator(), &info, &allocInfo, &m_Image, &m_Memory, nullptr));
}

Image::~Image() { vmaDestroyImage(Instance::Allocator(), m_Image, m_Memory); }

Image::Image(Image&& other)
{
	m_Image = other.m_Image;
	other.m_Image = VK_NULL_HANDLE;
	m_Memory = other.m_Memory;
	other.m_Memory = VK_NULL_HANDLE;
}

Image& Image::operator=(Image&& other)
{
	this->~Image();

	m_Image = other.m_Image;
	other.m_Image = VK_NULL_HANDLE;
	m_Memory = other.m_Memory;
	other.m_Memory = VK_NULL_HANDLE;

	return *this;
}
