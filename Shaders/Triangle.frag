#version 450

layout(location = 0) in vec4 InColor;

layout(location = 0) out vec4 OutColorBuffer;

void main()
{
	OutColorBuffer = InColor;
}
