#include "PCH.h"

#include "RenderPass.h"

RenderPass::RenderPass(std::span<VkAttachmentDescription> attachments, std::span<Subpass> subpasses,
	std::span<VkSubpassDependency> dependencies)
{
	std::vector<VkSubpassDescription> subpassDescs;
	subpassDescs.reserve(subpasses.size());
	for (const auto& subpass : subpasses)
	{
		ASSERT(!subpass.Resolve.size() || subpass.Resolve.size() == subpass.Color.size(),
			"Resolve attachments must be the same number as color attachments");

		subpassDescs.push_back(VkSubpassDescription{ .pipelineBindPoint = subpass.BindPoint,
			.inputAttachmentCount = u32(subpass.Input.size()),
			.pInputAttachments = subpass.Input.data(),
			.colorAttachmentCount = u32(subpass.Color.size()),
			.pColorAttachments = subpass.Color.data(),
			.pResolveAttachments = subpass.Resolve.size() ? subpass.Resolve.data() : nullptr,
			.pDepthStencilAttachment = subpass.Depth ? &subpass.Depth.value() : nullptr,
			.preserveAttachmentCount = u32(subpass.Preserve.size()),
			.pPreserveAttachments = subpass.Preserve.data() });
	}

	VkRenderPassCreateInfo info{ .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = u32(attachments.size()),
		.pAttachments = attachments.data(),
		.subpassCount = u32(subpassDescs.size()),
		.pSubpasses = subpassDescs.data(),
		.dependencyCount = u32(dependencies.size()),
		.pDependencies = dependencies.data() };

	VkCall(vkCreateRenderPass(Instance::Device(), &info, nullptr, &m_Pass));
}

RenderPass::~RenderPass() { vkDestroyRenderPass(Instance::Device(), m_Pass, nullptr); }

RenderPass::RenderPass(RenderPass&& other)
{
	m_Pass = other.m_Pass;
	other.m_Pass = VK_NULL_HANDLE;
}

RenderPass& RenderPass::operator=(RenderPass&& other)
{
	this->~RenderPass();

	m_Pass = other.m_Pass;
	other.m_Pass = VK_NULL_HANDLE;

	return *this;
}
