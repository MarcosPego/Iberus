#version 460 core

in vec2 exUVs;

uniform sampler2D bloomIn;
uniform vec2 screenSize;
uniform vec2 blurDirection;
uniform float blurRadius;

layout(location = 0) out vec4 bloomColor;

/// 9-tap Gaussian blur (sigma=2, normalized)
const float weights[5] = float[5](0.204164, 0.180173, 0.123831, 0.066282, 0.027630);

void main(void)
{
	vec2 pixelSize = 1.0 / screenSize;
	vec2 dir = blurDirection * pixelSize * blurRadius;

	vec3 result = texture(bloomIn, exUVs).rgb * weights[0];
	result += texture(bloomIn, exUVs + dir).rgb * weights[1];
	result += texture(bloomIn, exUVs - dir).rgb * weights[1];
	result += texture(bloomIn, exUVs + dir * 2.0).rgb * weights[2];
	result += texture(bloomIn, exUVs - dir * 2.0).rgb * weights[2];
	result += texture(bloomIn, exUVs + dir * 3.0).rgb * weights[3];
	result += texture(bloomIn, exUVs - dir * 3.0).rgb * weights[3];
	result += texture(bloomIn, exUVs + dir * 4.0).rgb * weights[4];
	result += texture(bloomIn, exUVs - dir * 4.0).rgb * weights[4];

	bloomColor = vec4(result, 1.0);
}
