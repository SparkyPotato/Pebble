#version 450

layout(set = 0, binding = 0) uniform Matrix
{
	mat4 Model;
	mat4 View;
	mat4 Projection;
} MVP;

layout(location = 0) in vec2 InPosition;
layout(location = 1) in vec3 InColor;

layout(location = 0) out vec4 OutColor;

void main()
{
	gl_Position = MVP.Projection * MVP.View * MVP.Model * vec4(InPosition, 0.f, 1.f);
	OutColor = vec4(InColor, 1.f);
}
