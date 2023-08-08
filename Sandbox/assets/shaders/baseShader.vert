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

void main(void)
{
	exVerticies = inVerticies;
	exUVs = inUVs;
	exNormals = inNormals;

	vec4 meshPosition = vec4(inVerticies, 1.0);
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * meshPosition;
	//gl_Position =  ProjectionMatrix * ViewMatrix * meshPosition;
	//gl_Position = meshPosition;
}