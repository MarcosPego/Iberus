#version 330 core

layout(location = 0) in vec3 inVerticies;
layout(location = 1) in vec2 inUVs;
layout(location = 2) in vec3 inNormals;

out vec3 exVerticies;
out vec2 exUVs;
out vec3 exNormals;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out mat4 worldMatrix;

void main(void)
{
	exVerticies = inVerticies;
	exUVs = inUVs;
	exNormals = inNormals;

	worldMatrix = ProjectionMatrix * ViewMatrix;
	gl_Position = worldMatrix * ModelMatrix * vec4(inVerticies, 1.0);
}