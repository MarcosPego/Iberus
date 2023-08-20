#version 330 core

uniform sampler2D worldPosIn;   
uniform sampler2D diffuseIn;     
uniform sampler2D normalIn;     
uniform sampler2D uvsIn; 

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
	vec3 color = texture(diffuseIn, uvCoord).xyz;
	fragColor = vec4(color, 1.0);
}
