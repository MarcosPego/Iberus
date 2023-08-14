#version 330 core

in vec3 exVerticies;
in vec2 exUVs;
in vec3 exNormals;

uniform vec4 albedoColor;

uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D metallicTexture;

uniform sampler2D colorMap;  

in vec4 position;

layout (location = 0) out vec3 worldPosOut;   
layout (location = 1) out vec3 diffuseOut;     
layout (location = 2) out vec3 normalOut;     
layout (location = 3) out vec3 uvsOut;   

void main(void)
{
	worldPosOut     = exVerticies;					
	diffuseOut      = texture(albedoTexture, exUVs).xyz;	
	normalOut       = normalize(exNormals);					
	uvsOut			= vec3(exUVs, 0.0);	
}
