#pragma once

#include "Instance.h"

struct Subpass
{
	VkPipelineBindPoint BindPoint;
	std::span<VkAttachmentReference> Input;
	std::span<VkAttachmentReference> Color;
	std::span<VkAttachmentReference> Resolve;
	std::optional<VkAttachmentReference> Depth;
	std::span<u32> Preserve;
};

class RenderPass
{
public:
	RenderPass() = default;
	RenderPass(std::span<VkAttachmentDescription> attachments, std::span<Subpass> subpasses,
		std::span<VkSubpassDependency> dependencies);
	~RenderPass();

	RenderPass(const RenderPass& other) = delete;
	RenderPass& operator=(const RenderPass& other) = delete;

	RenderPass(RenderPass&& other);
	RenderPass& operator=(RenderPass&& other);

	VkRenderPass GetHandle() const { return m_Pass; }

private:
	VkRenderPass m_Pass = VK_NULL_HANDLE;
};
