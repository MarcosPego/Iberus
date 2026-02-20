#version 330 core

in vec3 exVerticies;
in vec2 exUVs;
in vec3 exNormals;

uniform vec4 albedoColor;

uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D metallicTexture;

in vec4 position;

layout(location = 0) out vec4 diffuseColor;

void main(void)
{
	diffuseColor = texture(albedoTexture, exUVs) * albedoColor;
}
