#include "PCH.h"

#include "Pipeline.h"

Pipeline::Pipeline(std::span<Shader> shaders, const VertexInput& vertexInput, const Viewport& viewport,
	const Rasterizer& rasterizer, const Multisample& multisample, const DepthStencil& depthStencil,
	const BlendState& blendState, const DynamicState& dynamicState, const PipelineLayout& layout,
	const RenderPass& renderPass, u32 subpass)
{
	std::vector<VkPipelineShaderStageCreateInfo> stages;
	stages.reserve(shaders.size());
	for (const auto& shader : shaders)
	{
		stages.push_back(shader.GetInfo());
	}

	VkGraphicsPipelineCreateInfo info{ .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = u32(stages.size()),
		.pStages = stages.data(),
		.pVertexInputState = &vertexInput.GetInputInfo(),
		.pInputAssemblyState = &vertexInput.GetAssemblyInfo(),
		.pViewportState = &viewport.GetInfo(),
		.pRasterizationState = &rasterizer.GetInfo(),
		.pMultisampleState = &multisample.GetInfo(),
		.pDepthStencilState = &depthStencil.GetInfo(),
		.pColorBlendState = &blendState.GetInfo(),
		.pDynamicState = &dynamicState.GetInfo(),
		.layout = layout.GetHandle(),
		.renderPass = renderPass.GetHandle(),
		.subpass = subpass };

	VkCall(vkCreateGraphicsPipelines(Instance::Device(), VK_NULL_HANDLE, 1, &info, nullptr, &m_Pipeline));
}

Pipeline::~Pipeline() { vkDestroyPipeline(Instance::Device(), m_Pipeline, nullptr); }

Pipeline::Pipeline(Pipeline&& other)
{
	m_Pipeline = other.m_Pipeline;
	other.m_Pipeline = VK_NULL_HANDLE;
}

Pipeline& Pipeline::operator=(Pipeline&& other)
{
	this->~Pipeline();

	m_Pipeline = other.m_Pipeline;
	other.m_Pipeline = VK_NULL_HANDLE;

	return *this;
}
