#pragma once

#include "Instance.h"

#include "Image.h"
#include "RenderPass.h"

class Framebuffer
{
public:
	Framebuffer() = default;
	Framebuffer(const RenderPass& renderPass, glm::u32vec2 size, u32 layers, std::span<const ImageView*> attachments);
	~Framebuffer();

	Framebuffer(const Framebuffer& other) = delete;
	Framebuffer& operator=(const Framebuffer& other) = delete;

	Framebuffer(Framebuffer&& other);
	Framebuffer& operator=(Framebuffer&& other);

	VkFramebuffer GetHandle() const { return m_Framebuffer; }

private:
	VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
};
