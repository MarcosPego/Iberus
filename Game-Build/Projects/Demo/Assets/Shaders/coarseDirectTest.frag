#version 460 core

in vec2 exUVs;

uniform vec2 screenSize;

layout(location = 0) out vec3 worldPosOut;
layout(location = 1) out vec3 diffuseOut;
layout(location = 2) out vec3 normalOut;
layout(location = 3) out vec3 uvsOut;

uniform sampler2D worldPosIn;
uniform sampler2D normalIn;
uniform sampler2D uvsIn;

void main()
{
	// Minimal test: output gradient directly, no texture read. If this works, FBO draw is fine.
	float gradient = gl_FragCoord.x / screenSize.x;
	diffuseOut = vec3(gradient, gradient, gradient);

	vec2 uv = gl_FragCoord.xy / screenSize;
	worldPosOut = texture(worldPosIn, uv).xyz;
	normalOut = texture(normalIn, uv).xyz;
	uvsOut = texture(uvsIn, uv).xyz;
}
