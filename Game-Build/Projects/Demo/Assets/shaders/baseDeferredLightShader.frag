#version 460 core

uniform sampler2D worldPosIn;   
uniform sampler2D diffuseIn;     
uniform sampler2D normalIn;     
uniform sampler2D uvsIn; 

//uniform vec4 albedoColor;

//uniform sampler2D albedoTexture;
//uniform sampler2D normalTexture;
//uniform sampler2D metallicTexture;

//layout (location = 0) out vec4 fragColor;    

layout (location = 0) out vec3 worldPosOut;   
layout (location = 1) out vec3 diffuseOut;     
layout (location = 2) out vec3 normalOut;     
layout (location = 3) out vec3 uvsOut;  

struct Light {
	// 1 = PointLight, 2 = SpotLight, 3 = DirLight, 4 = AreaLight
	int type;
	vec3 color;
	float intensity;

	// Point Light
	vec3 position;

	float constant;
	float linear;
	float quadratic;
	
	// Spot Light
	float angel;
	float cutoffDiameter;

	float range;

	// Directional Light
	vec3 direction;
};

uniform Light lights[32];
uniform int lightCount;

uniform vec2 screenSize;

uniform vec3 cameraPos;

vec2 CalcUVCoord() {
    return gl_FragCoord.xy / screenSize;
}

// Light type: 1=Point, 2=Spot, 3=Dir, 4=Area
vec3 CalcLightContribution(vec3 fragPos, vec3 normalizedNormal, int idx) {
	vec3 lightBaseColor = lights[idx].color * lights[idx].intensity;
	vec3 lightDir;
	float attenuation = 1.0;

	if (lights[idx].type == 3) {
		// Directional: direction points from light toward surface (e.g. sun rays)
		lightDir = normalize(-lights[idx].direction);
	} else {
		// Point or Spot: direction from fragment to light
		vec3 toLight = lights[idx].position - fragPos;
		float dist = length(toLight);
		lightDir = toLight / max(dist, 0.0001);

		// Range cull
		if (lights[idx].range > 0.0 && dist > lights[idx].range) {
			return vec3(0.0);
		}

		// Attenuation: 1 / (c + l*d + q*d*d)
		attenuation = 1.0 / (lights[idx].constant + lights[idx].linear * dist + lights[idx].quadratic * dist * dist);

		// Spot cone (type 2)
		if (lights[idx].type == 2) {
			vec3 spotDir = normalize(-lights[idx].direction);
			float cosTheta = dot(lightDir, spotDir);
			float cutoff = cos(radians(lights[idx].angel));
			if (cosTheta < cutoff) {
				return vec3(0.0);
			}
		}
	}

	float diff = max(dot(normalizedNormal, lightDir), 0.0);
	return diff * lightBaseColor * attenuation;
}

void main(void)
{
	vec2 uvCoord = CalcUVCoord();

	vec3 fragPos = texture(worldPosIn, uvCoord).xyz;
	vec3 geomNormal = texture(normalIn, uvCoord).xyz;

	// Early-out: skip lighting for background/sky pixels (no geometry rendered here).
	// Clear color yields worldPos≈0 and normal≈0 for empty pixels.
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
		lightColor += CalcLightContribution(fragPos, normalizedNormal, i);
	}

	vec3 color = texture(diffuseIn, uvCoord).xyz;
	vec3 emissive = texture(uvsIn, uvCoord).xyz;
	vec3 fragColor = lightColor * color + emissive;

	worldPosOut = fragPos;
	diffuseOut = fragColor;
	normalOut = geomNormal;
	uvsOut = texture(uvsIn, uvCoord).xyz;
}
