#version 330 core

in vec3 exVerticies;
in vec2 exUVs;
in vec3 exNormals;

uniform vec4 albedoColor;

uniform sampler2D worldPosIn;   
uniform sampler2D diffuseIn;     
uniform sampler2D normalIn;     
uniform sampler2D uvsIn; 

uniform float exposure;
uniform float autoExposureMultiplier;
uniform bool autoExpose = false;
uniform float avgLuminance;
uniform float gamma;

in vec4 position;

layout(location = 0) out vec4 diffuseColor;


uniform vec2 screenSize;
vec2 CalcUVCoord() {
    return gl_FragCoord.xy / screenSize;
	//  return (gl_FragCoord.xy -.5 * screenSize.xy) / screenSize.y;
}

void main(void)
{
	vec2 uvCoord = CalcUVCoord();

	vec3 hdrColor = texture(diffuseIn, uvCoord).rgb;

	float e = exposure;
	/*if (autoExpose) {
		float lmax = 9.6 * avgLuminance;
		e = autoExposureMultiplier/lmax;
	}*/
    vec3 result = vec3(1.0) - exp(-hdrColor * e);    
    result = pow(result, vec3(1.0 / gamma));
	diffuseColor = vec4(result, 1.0f);
}
