#pragma once

#include "Instance.h"

class Viewport
{
public:
	Viewport() = default;
	Viewport(glm::vec2 offset, glm::vec2 size, glm::vec2 depthRange, VkRect2D scissor);

	Viewport(const Viewport& other);
	Viewport& operator=(const Viewport& other);

	const VkPipelineViewportStateCreateInfo& GetInfo() const { return m_Info; }

	const VkViewport& GetViewport() const { return m_Viewport; }
	const VkRect2D& GetScissor() const { return m_Scissor; }

private:
	VkViewport m_Viewport;
	VkRect2D m_Scissor;
	VkPipelineViewportStateCreateInfo m_Info{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
};

class Rasterizer
{
public:
	Rasterizer() = default;
	Rasterizer(VkFrontFace frontFace, VkBool32 clampDepth = VK_FALSE, VkBool32 discard = VK_FALSE,
		VkPolygonMode mode = VK_POLYGON_MODE_FILL, float lineWidth = 1.f, VkBool32 cull = VK_FALSE,
		VkBool32 depthBias = VK_FALSE, float depthBiasConstant = 0.f, float depthBiasClamp = 0.f,
		float depthBiasSlope = 0.f);

	const VkPipelineRasterizationStateCreateInfo& GetInfo() const { return m_Info; }

private:
	VkPipelineRasterizationStateCreateInfo m_Info{ .sType =
													   VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
};

class Multisample
{
public:
	Multisample(VkBool32 sampleShading = VK_FALSE, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT,
		float minSampleShading = 1.f, VkBool32 alphaCoverage = VK_FALSE, VkBool32 alphaOne = VK_FALSE);

	const VkPipelineMultisampleStateCreateInfo& GetInfo() const { return m_Info; }

private:
	VkPipelineMultisampleStateCreateInfo m_Info{ .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
};

class DepthStencil
{
public:
	DepthStencil(VkBool32 enable = VK_FALSE, VkBool32 write = VK_FALSE, VkCompareOp compareOp = VK_COMPARE_OP_LESS,
		VkBool32 boundsTest = VK_FALSE, float minBound = 0.f, float maxBound = 1.f, bool stencil = VK_FALSE,
		VkStencilOpState front = VkStencilOpState{}, VkStencilOpState back = VkStencilOpState{});

	const VkPipelineDepthStencilStateCreateInfo& GetInfo() const { return m_Info; }

private:
	VkPipelineDepthStencilStateCreateInfo m_Info{ .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
};

class BlendState
{
public:
	BlendState() = default;
	BlendState(std::initializer_list<VkPipelineColorBlendAttachmentState> attachments, VkBool32 bitwise = VK_FALSE,
		VkLogicOp bitwiseOp = VK_LOGIC_OP_COPY, glm::vec4 blendConstants = { 0.f, 0.f, 0.f, 0.f });

	BlendState(const BlendState& other);
	BlendState& operator=(const BlendState& other);

	BlendState(BlendState&& other);
	BlendState& operator=(BlendState&& other);

	const VkPipelineColorBlendStateCreateInfo& GetInfo() const { return m_Info; }

private:
	VkPipelineColorBlendStateCreateInfo m_Info{ .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	std::vector<VkPipelineColorBlendAttachmentState> m_Attachments;
};

class DynamicState
{
public:
	DynamicState() = default;
	DynamicState(const std::initializer_list<VkDynamicState>& states);

	DynamicState(const DynamicState& other);
	DynamicState& operator=(const DynamicState& other);

	DynamicState(DynamicState&& other);
	DynamicState& operator=(DynamicState&& other);

	const VkPipelineDynamicStateCreateInfo& GetInfo() const { return m_Info; }

private:
	VkPipelineDynamicStateCreateInfo m_Info{ .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	std::vector<VkDynamicState> m_States;
};
