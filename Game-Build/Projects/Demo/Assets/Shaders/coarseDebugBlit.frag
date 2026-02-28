#version 460 core

in vec2 exUVs;

uniform sampler2D coarseTex;
uniform vec2 screenSize;
uniform int coarseScale;

layout(location = 0) out vec3 worldPosOut;
layout(location = 1) out vec3 diffuseOut;
layout(location = 2) out vec3 normalOut;
layout(location = 3) out vec3 uvsOut;

uniform sampler2D worldPosIn;
uniform sampler2D normalIn;
uniform sampler2D uvsIn;

void main()
{
	// Sample coarse using same logic as main pass: block coord -> texel
	ivec2 blockCoord = ivec2(floor(gl_FragCoord.xy / float(coarseScale)));
	ivec2 coarseSize = ivec2(screenSize / float(coarseScale));
	blockCoord = clamp(blockCoord, ivec2(0), coarseSize - 1);
	float v = texelFetch(coarseTex, blockCoord, 0).r;
	diffuseOut = vec3(v, v, v);

	// Pass through geometry for other channels (needed by next pass)
	vec2 uv = gl_FragCoord.xy / screenSize;
	worldPosOut = texture(worldPosIn, uv).xyz;
	normalOut = texture(normalIn, uv).xyz;
	uvsOut = texture(uvsIn, uv).xyz;
}
