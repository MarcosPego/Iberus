#version 330 core

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

void main(void)
{
	vec2 uvCoord = CalcUVCoord();

	vec3 position = texture(worldPosIn, uvCoord).xyz;
	vec3 normalizedNormal = normalize(texture(normalIn, uvCoord).xyz);

	float ambientStrength = 0.2;
	vec3 lightColor = ambientStrength * vec3(1.0, 1.0, 1.0);

	for (int i = 0; i < lightCount && i < 32; ++i) {
		vec3 lightBaseColor = lights[i].color * lights[i].intensity;
		vec3 lightDir = normalize(lights[i].position - position);
		float diff = max(dot(normalizedNormal, lightDir), 0.0);
		lightColor += diff * lightBaseColor;
	}

	vec3 color = texture(diffuseIn, uvCoord).xyz;
	vec3 fragColor = lightColor * color;

	worldPosOut     = texture(worldPosIn, uvCoord).xyz;				
	diffuseOut      = fragColor;	
	normalOut       = texture(normalIn, uvCoord).xyz;					
	uvsOut			= texture(uvsIn, uvCoord).xyz;	

	//fragColor = vec4(color, 1.0);	
	//fragColor = vec4(texture(normalIn, uvCoord).xyz, 1);
}
