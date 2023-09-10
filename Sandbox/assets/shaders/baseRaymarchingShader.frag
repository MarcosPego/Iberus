#version 330 core

in mat4 worldMatrix;

uniform sampler2D worldPosIn;   
uniform sampler2D diffuseIn;     
uniform sampler2D normalIn;     
uniform sampler2D uvsIn; 

struct SDFMesh {
	vec3 center;
	vec4 color; // For now only color, later I want textures too
	float radius;
	int type;
};

const int sdfMeshBufferSize = 32;
uniform SDFMesh sdfMeshes[sdfMeshBufferSize];

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

float sdBox(vec3 position, float size) {
	vec3 distance = abs(position) - size;
	return min(max(distance.x, max(distance.y, distance.z)), 0.0) + length(max(distance, 0.0));
}

///

vec4 finalColor = vec4(0,0,0,0);

float distanceField(vec3 position) {
	float result =  2^128;
	for (int i = 0; i < sdfMeshBufferSize; i++) {
		SDFMesh mesh = sdfMeshes[i];
		if (mesh.type <= 0) {
			break;
		}

		float resultingT = result;
		if (mesh.type == 1) {
			resultingT = sdSphere(position - mesh.center, mesh.radius);

		} else if (mesh.type == 2) {
			resultingT = sdBox(position - mesh.center, mesh.radius);
		}

		if (resultingT < result) {
			finalColor = mesh.color;
			result = resultingT;
		}
	}
	return result;
}

vec3 getNormal(vec3 position) {
	const vec2 offset = vec2(0.001, 0.0);
	return vec3(
		-(distanceField(position + offset.xyy) - distanceField(position - offset.xyy)), // not sure why the x has to be inverted
		distanceField(position + offset.yxy) - distanceField(position - offset.yxy),
		distanceField(position + offset.yyx) - distanceField(position - offset.yyx)
	);
}

float raymarching(vec3 origin, vec3 direction) {
	float t = 0;
	const int maxIteration = 128;
	float maxDistance = 100.0f;

	for (int i = 0; i < maxIteration; i++) {
		if (t > maxDistance) {
			return -1.0;
		}

		vec3 position = origin + direction * t;
		float distance = distanceField(position);

		if (distance < 0.01) {
			return t;
		}
		t += distance;
	}

	return -1.0;
}

/// 

void main(void)
{
	mat4 camToWorld = inverse(worldMatrix);
	vec2 uvCoord = CalcUVCoord();
	
	vec4 nearPos4 = camToWorld * vec4(uvCoord.xy * 2 - 1, -1, 1);
    vec4 farPos4 = camToWorld * vec4(uvCoord.xy * 2 - 1, +1, 1);

	vec3 nearpos = nearPos4.xyz / nearPos4.w;
    vec3 farpos = farPos4.xyz / farPos4.w;

    vec3 rayOrigin = nearpos;
    vec3 rayDirection = normalize(farpos - nearpos);

	vec3 normal = vec3(0,0,0);
	vec3 pos = vec3(0,0,0);

	float t = raymarching(rayOrigin, rayDirection);
	if (t >= 0.0) {
		pos = rayOrigin + rayDirection * t;
		normal = normalize(getNormal(pos));
	} else {
		finalColor = vec4(0, 0, 0, 0);
	}

	worldPosOut     = texture(worldPosIn, uvCoord).xyz + pos;				
	diffuseOut      = texture(diffuseIn, uvCoord).xyz + finalColor.xyz;	
	normalOut       = texture(normalIn, uvCoord).xyz + normal;					
	uvsOut			= texture(uvsIn, uvCoord).xyz;	
}
