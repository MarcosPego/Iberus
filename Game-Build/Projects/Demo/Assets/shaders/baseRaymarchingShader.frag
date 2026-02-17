#version 330 core

in mat4 worldMatrix;

uniform sampler2D worldPosIn;   
uniform sampler2D diffuseIn;     
uniform sampler2D normalIn;     
uniform sampler2D uvsIn;
uniform sampler2D depthIn;

struct SDFPart {
	vec3 center;
	vec4 color; // For now only color, later I want textures too
	float radius;
	int type;
	vec3 endpoint; // For capsule: segment endpoint in world space
};

const int sdfPartBufferSize = 16;
struct SDFMesh {
	int size;
	vec3 boundCenter;
	float boundRadius;
	SDFPart sdfParts[sdfPartBufferSize];
};

const int sdfMeshBufferSize = 4;
uniform SDFMesh sdfMeshes[sdfMeshBufferSize];

layout (location = 0) out vec3 worldPosOut;   
layout (location = 1) out vec3 diffuseOut;     
layout (location = 2) out vec3 normalOut;     
layout (location = 3) out vec3 uvsOut;  

uniform vec2 screenSize;
uniform vec3 cameraPos;
uniform mat4 cameraToWorld;
uniform int debugRayMode;  // 0=normal, 1=rayDir, 2=rayOrigin, 3=hitMiss, 4=sdfAtOrigin, 5=ndc

vec2 CalcUVCoord() {
  return gl_FragCoord.xy / screenSize;
}

vec2 CalcNDC() {
  return (gl_FragCoord.xy - 0.5) / screenSize * 2.0 - 1.0;
}

/// Distance Field Functions

float sdSphere(vec3 position, float size) {
	return length(position) - size;
}

float sdBox(vec3 position, float size) {
	vec3 distance = abs(position) - size;
	return min(max(distance.x, max(distance.y, distance.z)), 0.0) + length(max(distance, 0.0));
}

float sdCapsule(vec3 p, vec3 a, vec3 b, float r) {
	vec3 pa = p - a;
	vec3 ba = b - a;
	float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
	return length(pa - ba * h) - r;
}

///

vec4 finalColor = vec4(0,0,0,0);

// Polynomial smooth min (much cheaper than exp/log, similar look)
float smootMin(float a, float b, float k) {
	float h = max(k - abs(a - b), 0.0) / k;
	return min(a, b) - h * h * k * 0.25;
}

float distanceField(vec3 position) {
	float result = 1e30;
	for (int i = 0; i < sdfMeshBufferSize; i++) {
		if (sdfMeshes[i].size <= 0) {
			continue;
		}
		// Bounding sphere culling: if we're farther from this mesh than current best hit, skip it
		float dToBound = length(position - sdfMeshes[i].boundCenter) - sdfMeshes[i].boundRadius;
		if (dToBound > result) {
			continue;
		}
		int meshSize = sdfMeshes[i].size;
		float resultingT = result;
		for (int j = 0; j < meshSize; j++) {
			SDFPart part = sdfMeshes[i].sdfParts[j];
			if (part.type <= 0) {
				break;
			}

			if (j == 0) {
				if (part.type == 1) {
					resultingT = sdSphere(position - part.center, part.radius);
				} else if (part.type == 2) {
					resultingT = sdBox(position - part.center, part.radius);
				} else if (part.type == 3) {
					resultingT = sdCapsule(position, part.center, part.endpoint, part.radius);
				}

				if (resultingT < result) {
					finalColor = part.color;
					result = resultingT;
				}	
			} else {
				float _previousT = resultingT;
				if (part.type == 1) {
					resultingT = smootMin(sdSphere(position - part.center, part.radius), resultingT, 0.5f);
				} else if (part.type == 2) {
					resultingT = smootMin(sdBox(position - part.center, part.radius), resultingT, 0.4f);
				} else if (part.type == 3) {
					resultingT = smootMin(sdCapsule(position, part.center, part.endpoint, part.radius), resultingT, 0.5f);
				}
				if (resultingT < result) {
					finalColor = mix(finalColor, part.color, clamp(_previousT - resultingT, 0, 1));
					result = resultingT;
				}	
			}			
		}		
	}
	return result;
}

// Tetrahedron gradient - 4 evaluations instead of 6
vec3 getNormal(vec3 p) {
	vec2 e = vec2(0.001, -0.001);
	return normalize(
		e.xyy * distanceField(p + e.xyy) +
		e.yyx * distanceField(p + e.yyx) +
		e.yxy * distanceField(p + e.yxy) +
		e.xxx * distanceField(p + e.xxx)
	);
}

float raymarching(vec3 origin, vec3 direction) {
	float t = 0;
	const int maxIteration = 64;
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
		t += max(distance, 0.001);  // min step to avoid stepping backward (negative dist = inside SDF)
	}

	return -1.0;
}

/// 

void main(void)
{
	vec2 uvCoord = CalcUVCoord();
	vec2 ndc = CalcNDC();
	mat4 camToWorld = inverse(worldMatrix);

	vec4 nearPos4 = camToWorld * vec4(ndc, -1.0, 1.0);
	vec4 farPos4 = camToWorld * vec4(ndc, 1.0, 1.0);

	vec3 nearPos = nearPos4.xyz / nearPos4.w;
	vec3 farPos = farPos4.xyz / farPos4.w;

	vec3 rayOrigin = nearPos;
	vec3 rayDirection = normalize(farPos - nearPos);

	vec3 normal = vec3(0,0,0);
	vec3 pos = vec3(0,0,0);

	float t = raymarching(rayOrigin, rayDirection);
	if (t >= 0.0) {
		pos = rayOrigin + rayDirection * t;
		normal = normalize(getNormal(pos));
	} else {
		finalColor = vec4(0, 0, 0, 0);
	}

	// Depth occlusion: use geometry depth buffer (precision-stable at any distance)
	if (t >= 0.0) {
		float geomDepth = texture(depthIn, uvCoord).r;
		vec4 clipPos = worldMatrix * vec4(pos, 1.0);
		float raymarchDepth = (clipPos.z / clipPos.w) * 0.5 + 0.5;
		const float depthBias = 0.0001;
		if (raymarchDepth > geomDepth + depthBias) {
			// SDF hit is behind geometry - treat as miss
			pos = vec3(0, 0, 0);
			normal = vec3(0, 0, 0);
			finalColor = vec4(0, 0, 0, 0);
			t = -1.0;
		}
	}

	// Choose surface: SDF in front occludes geometry, otherwise use geometry
	bool sdfInFront = (t >= 0.0);
	vec3 geomPos = texture(worldPosIn, uvCoord).xyz;
	vec3 geomDiffuse = texture(diffuseIn, uvCoord).xyz;
	vec3 geomNormal = texture(normalIn, uvCoord).xyz;

	// DEBUG: ray visualization - compare game vs editor viewport
	vec3 outColor;
	if (debugRayMode == 1) {
		outColor = 0.5 + 0.5 * rayDirection;
	} else if (debugRayMode == 2) {
		vec3 originOffset = rayOrigin - cameraPos;
		float len = length(originOffset);
		outColor = vec3(min(len * 0.1, 1.0), 0.0, 0.0);
	} else if (debugRayMode == 3) {
		outColor = (t >= 0.0) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	} else if (debugRayMode == 4) {
		float d = distanceField(rayOrigin);
		float v = 1.0 - smoothstep(0.0, 10.0, d);
		outColor = vec3(v, v, v);
	} else if (debugRayMode == 5) {
		outColor = vec3(ndc * 0.5 + 0.5, 0.0);
	} else {
		outColor = sdfInFront ? finalColor.xyz : geomDiffuse;
	}

	worldPosOut     = sdfInFront ? pos : geomPos;
	diffuseOut      = outColor;
	normalOut       = sdfInFront ? normal : geomNormal;
	uvsOut			= texture(uvsIn, uvCoord).xyz;
}
