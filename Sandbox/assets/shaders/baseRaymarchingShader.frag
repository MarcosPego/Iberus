#version 330 core

in mat4 worldMatrix;

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

uniform vec2 screenSize;
uniform vec3 cameraPos;
uniform mat4 cameraToWorld;

vec2 CalcUVCoord() {
    return gl_FragCoord.xy / screenSize;
}

/// Distance Field Functions

float sdSphere(vec3 position, float size) {
	return length(position) - size;
}

float distanceField(vec3 position) {
	float Sphere1 = sdSphere(position - vec3(0,0,35), 2.0);
	return Sphere1;
}

vec4 raymarching(vec3 origin, vec3 direction) {
	float t = 0;
	const int maxIteration = 128;
	float maxDistance = 100.0f;

	float minDistance = 50.0f;
	for (int i = 0; i < maxIteration; i++) {
		if (t > maxDistance) {
			return vec4(0,0,0,0);
		}

		vec3 position = origin + direction * t;
		float distance = distanceField(position);
		
		minDistance = min(distance, minDistance);

		if (distance < 0.01) {
			return vec4(0,0.5,0.75,1);
		}
		t += distance;
	}

	return vec4(0,0,0,0);
}

/// 

void main(void)
{
	mat4 camToWorld = inverse(worldMatrix);
	vec2 uvCoord = CalcUVCoord();
	vec2 ndc = uvCoord * 2.0 - 1.0;
	
	vec4 nearPos4 = camToWorld * vec4(uvCoord.xy * 2 - 1, -1, 1);
    vec4 farPos4 = camToWorld * vec4(uvCoord.xy * 2 - 1, +1, 1);

	vec3 nearpos = nearPos4.xyz / nearPos4.w;
    vec3 farpos = farPos4.xyz / farPos4.w;

    vec3 rayOrigin = cameraPos;
    vec3 rayDirection = normalize(farpos - nearpos);

	vec4 color = raymarching(rayOrigin, rayDirection);
	//fragColor = vec4(diffuseColor,1.0) + color;

	worldPosOut     = texture(worldPosIn, uvCoord).xyz;				
	diffuseOut      = texture(diffuseIn, uvCoord).xyz + color.xyz;	
	normalOut       = texture(normalIn, uvCoord).xyz ;					
	uvsOut			= texture(uvsIn, uvCoord).xyz ;	
}
