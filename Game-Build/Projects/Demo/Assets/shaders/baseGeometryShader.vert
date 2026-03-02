#version 460 core

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
	vec4 worldPos4 = ModelMatrix * vec4(inVerticies, 1.0);
	vec4 clipPos = ProjectionMatrix * ViewMatrix * worldPos4;

	// World-space position for deferred lighting
	exVerticies = worldPos4.xyz;
	exUVs = inUVs;
	// World-space normals (model matrix only; no view/projection)
	exNormals = normalize((ModelMatrix * vec4(inNormals, 0.0)).xyz);

	gl_Position = clipPos;
}