#version 460 core

in vec3 exVerticies;
in vec2 exUVs;
in vec3 exNormals;

uniform vec4 albedoColor;

uniform sampler2D worldPosIn;
uniform sampler2D diffuseIn;
uniform sampler2D normalIn;
uniform sampler2D uvsIn;
uniform sampler2D bloomIn;

uniform float exposure;
uniform float autoExposureMultiplier;
uniform bool autoExpose = false;
uniform float avgLuminance;
uniform float gamma;

uniform float bloomIntensity;

layout(location = 0) out vec4 diffuseColor;

uniform vec2 screenSize;

vec2 CalcUVCoord() {
	return gl_FragCoord.xy / screenSize;
}

void main(void)
{
	vec2 uvCoord = CalcUVCoord();

	vec3 hdrColor = texture(diffuseIn, uvCoord).rgb;
	vec3 bloom = texture(bloomIn, uvCoord).rgb;

	vec3 combined = hdrColor + bloom * bloomIntensity;

	float e = exposure;
	vec3 result = vec3(1.0) - exp(-combined * e);
	result = pow(result, vec3(1.0 / gamma));
	diffuseColor = vec4(result, 1.0);
}
