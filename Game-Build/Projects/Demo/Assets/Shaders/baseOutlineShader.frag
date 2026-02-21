#version 460 core

uniform sampler2D worldPosIn;
uniform sampler2D diffuseIn;
uniform sampler2D normalIn;
uniform sampler2D uvsIn;

uniform vec2 screenSize;

layout(location = 0) out vec3 worldPosOut;
layout(location = 1) out vec3 diffuseOut;
layout(location = 2) out vec3 normalOut;
layout(location = 3) out vec3 uvsOut;

vec2 CalcUVCoord() {
	return gl_FragCoord.xy / screenSize;
}

void main(void)
{
	vec2 uvCoord = CalcUVCoord();

	worldPosOut = texture(worldPosIn, uvCoord).xyz;
	diffuseOut = texture(diffuseIn, uvCoord).xyz;
	normalOut = texture(normalIn, uvCoord).xyz;
	uvsOut = texture(uvsIn, uvCoord).xyz;
}
