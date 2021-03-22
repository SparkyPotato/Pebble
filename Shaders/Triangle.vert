#version 450

layout(location = 0) in vec2 InPosition;
layout(location = 1) in vec3 InColor;

layout(location = 0) out vec4 OutColor;

void main()
{
	gl_Position = vec4(InPosition, 0.f, 1.f);
	OutColor = vec4(InColor, 1.f);
}
