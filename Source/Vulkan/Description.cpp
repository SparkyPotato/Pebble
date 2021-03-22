#include "PCH.h"

#include "Description.h"

Viewport::Viewport(glm::vec2 offset, glm::vec2 size, glm::vec2 depthRange, VkRect2D scissor)
{
	m_Viewport.x = offset.x;
	m_Viewport.y = offset.y;
	m_Viewport.width = size.x;
	m_Viewport.height = size.y;
	m_Viewport.minDepth = depthRange.x;
	m_Viewport.maxDepth = depthRange.y;

	m_Scissor = scissor;

	m_Info.viewportCount = 1;
	m_Info.pViewports = &m_Viewport;
	m_Info.scissorCount = 1;
	m_Info.pScissors = &m_Scissor;
}

Viewport::Viewport(const Viewport& other)
{
	m_Viewport = other.m_Viewport;
	m_Scissor = other.m_Scissor;
	m_Info = other.m_Info;

	m_Info.pViewports = &m_Viewport;
	m_Info.pScissors = &m_Scissor;
}

Viewport& Viewport::operator=(const Viewport& other)
{
	m_Viewport = other.m_Viewport;
	m_Scissor = other.m_Scissor;
	m_Info = other.m_Info;

	m_Info.pViewports = &m_Viewport;
	m_Info.pScissors = &m_Scissor;

	return *this;
}

Rasterizer::Rasterizer(VkFrontFace frontFace, VkBool32 clampDepth, VkBool32 discard, VkPolygonMode mode,
	float lineWidth, VkBool32 cull, VkBool32 depthBias, float depthBiasConstant, float depthBiasClamp,
	float depthBiasSlope)
{
	m_Info.depthClampEnable = clampDepth;
	m_Info.rasterizerDiscardEnable = discard;
	m_Info.polygonMode = mode;
	m_Info.lineWidth = lineWidth;
	m_Info.cullMode = cull ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
	m_Info.frontFace = frontFace;
	m_Info.depthBiasEnable = depthBias;
	m_Info.depthBiasConstantFactor = depthBiasConstant;
	m_Info.depthBiasClamp = depthBiasClamp;
	m_Info.depthBiasSlopeFactor = depthBiasSlope;
}

Multisample::Multisample(VkBool32 sampleShading, VkSampleCountFlagBits sampleCount, float minSampleShading,
	VkBool32 alphaCoverage, VkBool32 alphaOne)
{
	m_Info.sampleShadingEnable = sampleShading;
	m_Info.rasterizationSamples = sampleCount;
	m_Info.minSampleShading = minSampleShading;
	m_Info.alphaToCoverageEnable = alphaCoverage;
	m_Info.alphaToOneEnable = alphaOne;
}

DepthStencil::DepthStencil(VkBool32 enable, VkBool32 write, VkCompareOp compareOp, VkBool32 boundsTest, float minBound,
	float maxBound, bool stencil, VkStencilOpState front, VkStencilOpState back)
{
	m_Info.depthTestEnable = enable;
	m_Info.depthWriteEnable = write;
	m_Info.depthCompareOp = compareOp;
	m_Info.depthBoundsTestEnable = boundsTest;
	m_Info.minDepthBounds = minBound;
	m_Info.maxDepthBounds = maxBound;
	m_Info.stencilTestEnable = stencil;
	m_Info.front = front;
	m_Info.back = back;
}

BlendState::BlendState(std::initializer_list<VkPipelineColorBlendAttachmentState> attachments, VkBool32 bitwise,
	VkLogicOp bitwiseOp, glm::vec4 blendConstants)
{
	m_Attachments = attachments;
	m_Info.logicOpEnable = bitwise;
	m_Info.logicOp = bitwiseOp;
	m_Info.blendConstants[0] = blendConstants.x;
	m_Info.blendConstants[1] = blendConstants.y;
	m_Info.blendConstants[2] = blendConstants.z;
	m_Info.blendConstants[3] = blendConstants.w;
	m_Info.attachmentCount = u32(m_Attachments.size());
	m_Info.pAttachments = m_Attachments.data();
}

BlendState::BlendState(const BlendState& other) 
{
	m_Attachments = other.m_Attachments;
	m_Info = other.m_Info;
	m_Info.pAttachments = m_Attachments.data();
}

BlendState& BlendState::operator=(const BlendState& other) 
{
	m_Attachments = other.m_Attachments;
	m_Info = other.m_Info;
	m_Info.pAttachments = m_Attachments.data();

	return *this;
}

BlendState::BlendState(BlendState&& other) 
{
	m_Attachments = std::move(other.m_Attachments);
	m_Info = other.m_Info;
	m_Info.pAttachments = m_Attachments.data();
}

BlendState& BlendState::operator=(BlendState&& other) 
{
	m_Attachments = std::move(other.m_Attachments);
	m_Info = other.m_Info;
	m_Info.pAttachments = m_Attachments.data();

	return *this;
}

DynamicState::DynamicState(const std::initializer_list<VkDynamicState>& states) 
{
	m_States = states;
	m_Info.dynamicStateCount = u32(m_States.size());
	m_Info.pDynamicStates = m_States.data();
}

DynamicState::DynamicState(const DynamicState& other) 
{
	m_States = other.m_States;
	m_Info = other.m_Info;
	m_Info.pDynamicStates = m_States.data();
}

DynamicState& DynamicState::operator=(const DynamicState& other) 
{
	m_States = other.m_States;
	m_Info = other.m_Info;
	m_Info.pDynamicStates = m_States.data();

	return *this;
}

DynamicState::DynamicState(DynamicState&& other) 
{
	m_States = std::move(other.m_States);
	m_Info = other.m_Info;
	m_Info.pDynamicStates = m_States.data();
}

DynamicState& DynamicState::operator=(DynamicState&& other) 
{
	m_States = std::move(other.m_States);
	m_Info = other.m_Info;
	m_Info.pDynamicStates = m_States.data();

	return *this;
}
