#version 460 core

layout(location = 0) in vec3 inVerticies;
layout(location = 1) in vec2 inUVs;
layout(location = 2) in vec3 inNormals;

out vec2 exUVs;

void main(void)
{
	exUVs = inUVs;
	gl_Position = vec4(inVerticies, 1.0);
}
