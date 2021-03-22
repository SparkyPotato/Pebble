#include "PCH.h"

#include "VertexInput.h"

VertexInput::VertexInput(VkPrimitiveTopology topology, const std::initializer_list<VertexElement>& elements)
{
	m_Binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	m_InputInfo.vertexBindingDescriptionCount = 1;
	m_InputInfo.pVertexBindingDescriptions = &m_Binding;

	u32 offset = 0;
	for (const auto& element : elements)
	{
		VkVertexInputAttributeDescription attribute{
			.location = element.Location, .binding = 0, .format = element.Format, .offset = offset
		};
		switch (attribute.format)
		{
		case VK_FORMAT_R32_SFLOAT:
			offset += sizeof(float);
			break;
		case VK_FORMAT_R32G32_SFLOAT:
			offset += sizeof(float) * 2;
			break;
		case VK_FORMAT_R32G32B32_SFLOAT:
			offset += sizeof(float) * 3;
			break;
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			offset += sizeof(float) * 4;
			break;
		case VK_FORMAT_R8G8B8A8_UNORM:
			offset += sizeof(u8) * 4;
			break;
		default:
			CRITICAL("Unsupported VkFormat for vertex element!");
		}

		m_Attributes.push_back(attribute);
	}

	m_Binding.stride = offset;
	m_InputInfo.vertexAttributeDescriptionCount = u32(m_Attributes.size());
	m_InputInfo.pVertexAttributeDescriptions = m_Attributes.data();

	m_AssemblyInfo.topology = topology;
}

VertexInput::VertexInput(const VertexInput& other) 
{
	m_Binding = other.m_Binding;
	m_Attributes = other.m_Attributes;
	m_AssemblyInfo = other.m_AssemblyInfo;
	m_InputInfo = other.m_InputInfo;

	m_InputInfo.pVertexAttributeDescriptions = m_Attributes.data();
	m_InputInfo.pVertexBindingDescriptions = &m_Binding;
}

VertexInput::VertexInput(VertexInput&& other) 
{
	m_Binding = other.m_Binding;
	m_Attributes = std::move(other.m_Attributes);
	m_AssemblyInfo = other.m_AssemblyInfo;
	m_InputInfo = other.m_InputInfo;

	m_InputInfo.pVertexAttributeDescriptions = m_Attributes.data();
	m_InputInfo.pVertexBindingDescriptions = &m_Binding;
}

VertexInput& VertexInput::operator=(VertexInput&& other) 
{
	m_Binding = other.m_Binding;
	m_Attributes = other.m_Attributes;
	m_AssemblyInfo = other.m_AssemblyInfo;
	m_InputInfo = other.m_InputInfo;

	m_InputInfo.pVertexAttributeDescriptions = m_Attributes.data();
	m_InputInfo.pVertexBindingDescriptions = &m_Binding;

	return *this;
}

VertexInput& VertexInput::operator=(const VertexInput& other) 
{
	m_Binding = other.m_Binding;
	m_Attributes = std::move(other.m_Attributes);
	m_AssemblyInfo = other.m_AssemblyInfo;
	m_InputInfo = other.m_InputInfo;

	m_InputInfo.pVertexAttributeDescriptions = m_Attributes.data();
	m_InputInfo.pVertexBindingDescriptions = &m_Binding;

	return *this;
}
