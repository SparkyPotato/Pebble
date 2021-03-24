#version 450

layout(set = 0, binding = 1) uniform sampler2D Texture;

layout(location = 0) in vec2 InPosition;
layout(location = 1) in vec4 InColor;

layout(location = 0) out vec4 OutColorBuffer;

void main()
{
	OutColorBuffer = InColor * texture(Texture, InPosition);
}
