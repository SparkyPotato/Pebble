#include "PCH.h"

#include "Image.h"

ImageView::ImageView(VkImage vkImage, VkFormat format, VkImageViewType viewType, VkComponentMapping mapping,
	VkImageSubresourceRange range)
{
	VkImageViewCreateInfo info{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.image = vkImage,
		.viewType = viewType,
		.format = format,
		.components = mapping,
		.subresourceRange = range };

	VkCall(vkCreateImageView(Instance::Device(), &info, nullptr, &m_View));
}

ImageView::~ImageView()
{
	vkDestroyImageView(Instance::Device(), m_View, nullptr);
}

ImageView::ImageView(ImageView&& other)
{
	m_View = other.m_View;
	other.m_View = VK_NULL_HANDLE;
	m_Format = other.m_Format;
}

ImageView& ImageView::operator=(ImageView&& other)
{
	this->~ImageView();

	m_View = other.m_View;
	other.m_View = VK_NULL_HANDLE;
	m_Format = other.m_Format;

	return *this;
}
