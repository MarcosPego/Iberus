#version 460 core

/// After HDR, attachment 0 holds the tone-mapped color (bound to worldPosIn slot)
uniform sampler2D worldPosIn;

uniform vec2 screenSize;
uniform int pixelCount;

layout(location = 0) out vec4 diffuseColor;

vec2 CalcUVCoord() {
	return gl_FragCoord.xy / screenSize;
}

void main(void)
{
	vec2 uvCoord = CalcUVCoord();

	if (pixelCount <= 0) {
		diffuseColor = vec4(texture(worldPosIn, uvCoord).rgb, 1.0);
		return;
	}

	float px = float(pixelCount);
	vec2 quantizedUV = floor(uvCoord * px) / px + 0.5 / px;
	vec3 color = texture(worldPosIn, quantizedUV).rgb;
	diffuseColor = vec4(color, 1.0);
}
