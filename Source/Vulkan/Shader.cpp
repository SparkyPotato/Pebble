#include "PCH.h"

#include "Shader.h"

Shader::Shader(const std::string& filePath, VkShaderStageFlagBits stage, const std::string& entry) : m_Entry(entry)
{
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	if (!file)
	{
		CRITICAL("Failed to open SPIRV file '{}'", filePath);
	}

	u64 size = file.tellg();
	std::vector<char> code(size);
	file.seekg(0);
	file.read(code.data(), size);

	VkShaderModuleCreateInfo info{ .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const u32*>(code.data()) };
	VkCall(vkCreateShaderModule(Instance::Device(), &info, nullptr, &m_Module));

	m_Stage.stage = stage;
	m_Stage.module = m_Module;
	m_Stage.pName = m_Entry.c_str();
}

Shader::~Shader() { vkDestroyShaderModule(Instance::Device(), m_Module, nullptr); }

Shader::Shader(Shader&& other)
{
	m_Module = other.m_Module;
	other.m_Module = VK_NULL_HANDLE;
	m_Stage = other.m_Stage;
	m_Entry = std::move(other.m_Entry);
	m_Stage.pName = m_Entry.c_str(); // Should not have changed due to the move, but let's be safe
}

Shader& Shader::operator=(Shader&& other)
{
	this->~Shader();

	m_Module = other.m_Module;
	other.m_Module = VK_NULL_HANDLE;
	m_Stage = other.m_Stage;
	m_Entry = std::move(other.m_Entry);
	m_Stage.pName = m_Entry.c_str(); // Should not have changed due to the move, but let's be safe

	return *this;
}
