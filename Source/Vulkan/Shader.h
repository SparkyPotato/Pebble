#pragma once

#include "Instance.h"

class Shader
{
public:
	Shader() = default;
	Shader(const std::string& filePath, VkShaderStageFlagBits stage, const std::string& entry = "main");
	~Shader();

	Shader(const Shader& other) = delete;
	Shader& operator=(const Shader& other) = delete;

	Shader(Shader&& other);
	Shader& operator=(Shader&& other);

	const VkPipelineShaderStageCreateInfo& GetInfo() const { return m_Stage; }

private:
	VkShaderModule m_Module = VK_NULL_HANDLE;
	VkPipelineShaderStageCreateInfo m_Stage{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	std::string m_Entry;
};
