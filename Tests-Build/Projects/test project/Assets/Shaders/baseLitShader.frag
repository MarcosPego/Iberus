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
	vec3 lightBaseColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 lightPos = vec3(-15.0f, 0.0f, 29.0f);

	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightBaseColor;

	vec3 normalizedNormal = normalize(exNormals);
	vec3 lightDir = normalize(lightPos - vec3(position));
	float diff = max(dot(normalizedNormal, lightDir), 0.0);
	vec3 diffuse = diff * lightBaseColor;
	
	vec3 lightColor = (ambient + diffuse);

	diffuseColor = vec4(lightColor,1) * texture(albedoTexture, exUVs) * albedoColor;
}
