#version 460 core

uniform sampler2D worldPosIn;
uniform sampler2D diffuseIn;
uniform sampler2D normalIn;
uniform sampler2D uvsIn;

layout(location = 0) out vec3 worldPosOut;
layout(location = 1) out vec3 diffuseOut;
layout(location = 2) out vec3 normalOut;
layout(location = 3) out vec3 uvsOut;

void main(void)
{
	vec2 uv = gl_FragCoord.xy / vec2(textureSize(worldPosIn, 0));
	worldPosOut = texture(worldPosIn, uv).rgb;
	diffuseOut = texture(diffuseIn, uv).rgb;
	normalOut = texture(normalIn, uv).rgb;
	uvsOut = texture(uvsIn, uv).rgb;
}
