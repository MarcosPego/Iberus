#version 330 core

in vec3 exVerticies;
in vec2 exUVs;
in vec3 exNormals;

uniform vec4 albedoColor;

uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D metallicTexture;


void main(void)
{
	vec3 color = (exNormals + vec3(1.0)) * 0.5;
	//gl_FragColor = vec4(exNormals,1.0) * albedoColor;
	gl_FragColor = texture(albedoTexture, exUVs) * albedoColor;
}
