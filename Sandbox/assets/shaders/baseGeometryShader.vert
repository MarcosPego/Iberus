#version 330 core

layout(location = 0) in vec3 inVerticies;
layout(location = 1) in vec2 inUVs;
layout(location = 2) in vec3 inNormals;

out vec3 exVerticies;
out vec2 exUVs;
out vec3 exNormals;

out vec4 position;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{
	mat4 worlMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
	vec4 meshPosition = vec4(inVerticies, 1.0);
	position = worlMatrix * meshPosition;

	exVerticies = position.xyz;
	exUVs = inUVs;
	exNormals = (worlMatrix * vec4(inNormals, 0.0)).xyz;

	gl_Position = position;
}