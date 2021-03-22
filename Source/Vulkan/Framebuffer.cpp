#include "PCH.h"

#include "Framebuffer.h"

Framebuffer::Framebuffer(const RenderPass& renderPass, glm::u32vec2 size, u32 layers, std::span<const ImageView*> attachments)
{
	std::vector<VkImageView> imageViews;
	imageViews.reserve(attachments.size());
	for (const auto& view : attachments)
	{
		imageViews.push_back(view->GetHandle());
	}

	VkFramebufferCreateInfo info{ .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = renderPass.GetHandle(),
		.attachmentCount = u32(imageViews.size()),
		.pAttachments = imageViews.data(),
		.width = size.x,
		.height = size.y,
		.layers = layers };
	VkCall(vkCreateFramebuffer(Instance::Device(), &info, nullptr, &m_Framebuffer));
}

Framebuffer::~Framebuffer() { vkDestroyFramebuffer(Instance::Device(), m_Framebuffer, nullptr); }

Framebuffer::Framebuffer(Framebuffer&& other)
{
	m_Framebuffer = other.m_Framebuffer;
	other.m_Framebuffer = VK_NULL_HANDLE;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other)
{
	this->~Framebuffer();
	m_Framebuffer = other.m_Framebuffer;
	other.m_Framebuffer = VK_NULL_HANDLE;

	return *this;
}
