#version 460 core

uniform sampler2D worldPosIn;
uniform sampler2D diffuseIn;
uniform sampler2D normalIn;
uniform sampler2D uvsIn;

layout(location = 0) out vec3 worldPosOut;
layout(location = 1) out vec3 diffuseOut;
layout(location = 2) out vec3 normalOut;
layout(location = 3) out vec3 uvsOut;

struct Light {
	int type;
	vec3 color;
	float intensity;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	float angel;
	float cutoffDiameter;
	float range;
	vec3 direction;
};

uniform Light lights[32];
uniform int lightCount;

uniform vec2 screenSize;
uniform vec3 cameraPos;

uniform int toonCuts;
uniform float toonSteepness;
uniform float toonWrap;
uniform float toonRimWidth;

vec2 CalcUVCoord() {
	return gl_FragCoord.xy / screenSize;
}

float ToonStep(float diffuseAmount) {
	if (toonCuts <= 0) {
		return diffuseAmount;
	}
	float stepped = floor(diffuseAmount * float(toonCuts)) / float(toonCuts);
	return clamp(stepped, 0.0, 1.0);
}

vec3 CalcToonLightContribution(vec3 fragPos, vec3 normalizedNormal, int idx) {
	vec3 lightBaseColor = lights[idx].color * lights[idx].intensity;
	vec3 lightDir;
	float attenuation = 1.0;

	if (lights[idx].type == 3) {
		lightDir = normalize(-lights[idx].direction);
	} else {
		vec3 toLight = lights[idx].position - fragPos;
		float dist = length(toLight);
		lightDir = toLight / max(dist, 0.0001);

		if (lights[idx].range > 0.0 && dist > lights[idx].range) {
			return vec3(0.0);
		}

		attenuation = 1.0 / (lights[idx].constant + lights[idx].linear * dist + lights[idx].quadratic * dist * dist);

		if (lights[idx].type == 2) {
			vec3 spotDir = normalize(-lights[idx].direction);
			float cosTheta = dot(lightDir, spotDir);
			float cutoff = cos(radians(lights[idx].angel));
			if (cosTheta < cutoff) {
				return vec3(0.0);
			}
		}
	}

	float NdotL = max(dot(normalizedNormal, lightDir), 0.0);
	float diffuseAmount = (NdotL + toonWrap) * toonSteepness;
	float stepped = ToonStep(diffuseAmount);
	return stepped * lightBaseColor * attenuation;
}

void main(void)
{
	vec2 uvCoord = CalcUVCoord();

	vec3 fragPos = texture(worldPosIn, uvCoord).xyz;
	vec3 geomNormal = texture(normalIn, uvCoord).xyz;

	if (dot(geomNormal, geomNormal) < 0.001) {
		vec3 ambient = 0.1 * vec3(1.0, 1.0, 1.0);
		worldPosOut = fragPos;
		diffuseOut = ambient;
		normalOut = geomNormal;
		uvsOut = texture(uvsIn, uvCoord).xyz;
		return;
	}

	vec3 normalizedNormal = normalize(geomNormal);

	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
	vec3 lightColor = ambient;

	for (int i = 0; i < lightCount && i < 32; ++i) {
		lightColor += CalcToonLightContribution(fragPos, normalizedNormal, i);
	}

	vec3 viewDir = normalize(cameraPos - fragPos);
	float NdotV = max(dot(normalizedNormal, viewDir), 0.0);
	float rim = pow(1.0 - NdotV, toonRimWidth);
	lightColor += rim * vec3(0.2, 0.2, 0.3);

	vec3 color = texture(diffuseIn, uvCoord).xyz;
	vec3 emissive = texture(uvsIn, uvCoord).xyz;
	vec3 fragColor = lightColor * color + emissive;

	worldPosOut = fragPos;
	diffuseOut = fragColor;
	normalOut = geomNormal;
	uvsOut = texture(uvsIn, uvCoord).xyz;
}
