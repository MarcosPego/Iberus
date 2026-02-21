#version 460 core

in vec2 exUVs;

uniform sampler2D colorIn;
uniform float bloomThreshold;

layout(location = 0) out vec4 bloomColor;

vec3 BrightPass(vec3 color) {
	return max(vec3(0.0), color - bloomThreshold);
}

void main(void)
{
	vec3 color = texture(colorIn, exUVs).rgb;
	bloomColor = vec4(BrightPass(color), 1.0);
}
