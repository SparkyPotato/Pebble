#pragma once

#include "Instance.h"

class ImageView
{
public:
	ImageView() = default;
	ImageView(VkImage vkImage, VkFormat format, VkImageViewType viewType, VkComponentMapping mapping, VkImageSubresourceRange range);
	~ImageView();

	ImageView(const ImageView& other) = delete;
	ImageView& operator=(const ImageView& other) = delete;

	ImageView(ImageView&& other);
	ImageView& operator=(ImageView&& other);

private:
	VkImageView m_View = VK_NULL_HANDLE;
	VkFormat m_Format = VK_FORMAT_UNDEFINED;
};
