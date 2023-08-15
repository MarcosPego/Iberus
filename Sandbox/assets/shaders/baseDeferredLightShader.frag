#version 330 core

uniform sampler2D worldPosOut;   
uniform sampler2D diffuseOut;     
uniform sampler2D normalOut;     
uniform sampler2D uvsOut; 

//uniform vec4 albedoColor;

//uniform sampler2D albedoTexture;
//uniform sampler2D normalTexture;
//uniform sampler2D metallicTexture;

layout (location = 0) out vec4 fragColor;    

uniform vec2 screenSize;

uniform vec3 cameraPos;

vec2 CalcUVCoord() {
    return gl_FragCoord.xy / screenSize;
}

void main(void)
{
	vec2 uvCoord = CalcUVCoord();
	vec3 color = texture(diffuseOut, uvCoord).xyz;
	fragColor = vec4(color, 1.0);
}
