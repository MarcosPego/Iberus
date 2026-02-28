#version 460 core

in mat4 worldMatrix;

const int maxCreatures = 256;
const int poolBudget = 4096;

struct SDFPart {
	vec4 center;
	vec4 color;
	float radius;
	int type;
	uint blendGroupMask;
	vec4 endpoint;
	vec4 volumeTexture;
};

struct CreatureHeader {
	int partOffset;
	int partCount;
	vec4 boundCenter;
	float boundRadius;
	int _pad;
};

layout(std430, binding = 0) buffer SDFBlock {
	int creatureCount;
	int totalPartCount;
	int _pad[2];
	CreatureHeader headers[maxCreatures];
	SDFPart parts[poolBudget];
};

// Single output for coarse FBO; MRT for target (outputToTarget=1)
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 diffuseOut;
layout(location = 2) out vec3 normalOut;
layout(location = 3) out vec3 uvsOut;

uniform vec2 screenSize;
uniform usamplerBuffer tileIndicesBuffer;
uniform sampler2D worldPosIn;
uniform sampler2D normalIn;
uniform sampler2D uvsIn;
uniform int tilesX;
uniform int tilesY;
uniform int tileSize;
uniform int coarseScale;  // 8 for 8x8 blocks
uniform int coarseDebugMode;  // 0=hit mask, 1=tileCount/64, 2=gradient, 5=pipeline test (white)
uniform int outputToTarget;   // 1=drawing to target at full res (gradient uses full range)

const int tileStride = 65;

vec2 CalcNDC() {
	// outputToTarget=1: full res. outputToTarget=0: coarse res - scale up for ray
	vec2 pixel = (outputToTarget != 0) ? gl_FragCoord.xy : (gl_FragCoord.xy * float(coarseScale));
	return (pixel - 0.5) / screenSize * 2.0 - 1.0;
}

bool rayIntersectsSphere(vec3 O, vec3 D, vec3 center, float radius, float maxT) {
	vec3 L = center - O;
	float tCa = dot(L, D);
	float lSq = dot(L, L);
	float rSq = radius * radius;
	float dSq = lSq - tCa * tCa;
	if (dSq > rSq) {
		return false;
	}
	float tHcSq = rSq - dSq;
	float tHc = sqrt(max(0.0, tHcSq));
	float t0 = tCa - tHc;
	float t1 = tCa + tHc;
	if (t1 < 0.0 || t0 > maxT) {
		return false;
	}
	return true;
}

bool rayMayHitSDFForTile(int tileIdx, vec3 rayOrigin, vec3 rayDir, float maxDist) {
	int count = int(texelFetch(tileIndicesBuffer, tileIdx * tileStride).r);
	const float boundMargin = 1.05;
	for (int k = 0; k < count; k++) {
		int i = int(texelFetch(tileIndicesBuffer, tileIdx * tileStride + 1 + k).r);
		if (i >= creatureCount || headers[i].partCount <= 0) {
			continue;
		}
		vec3 center = headers[i].boundCenter.xyz;
		float r = headers[i].boundRadius * boundMargin;
		if (rayIntersectsSphere(rayOrigin, rayDir, center, r, maxDist)) {
			return true;
		}
	}
	return false;
}

void main() {
	// Debug 5: pipeline test - output solid white to verify coarse FBO write + main pass read
	if (coarseDebugMode == 5) {
		float v = 1.0;
		fragColor = vec4(v, v, v, 1.0);
		if (outputToTarget != 0) {
			vec2 uv = gl_FragCoord.xy / screenSize;
			fragColor = vec4(texture(worldPosIn, uv).xyz, 1.0);
			diffuseOut = vec3(v, v, v);
			normalOut = texture(normalIn, uv).xyz;
			uvsOut = texture(uvsIn, uv).xyz;
		} else {
			diffuseOut = vec3(0.0);
			normalOut = vec3(0.0);
			uvsOut = vec3(0.0);
		}
		return;
	}
	// Debug 2: output gradient. When outputToTarget=1 we draw to target FBO (4 attachments) - must write all 4.
	// When outputToTarget=0 we draw to coarse FBO (1 attachment) - only fragColor (location 0) is used.
	if (coarseDebugMode == 2) {
		float v = (outputToTarget != 0)
			? (gl_FragCoord.x / screenSize.x)
			: (gl_FragCoord.x / (screenSize.x / float(coarseScale)));
		fragColor = vec4(v, v, v, 1.0);
		if (outputToTarget != 0) {
			vec2 uv = gl_FragCoord.xy / screenSize;
			fragColor = vec4(texture(worldPosIn, uv).xyz, 1.0);  // location 0 = worldPos (pass-through)
			diffuseOut = vec3(v, v, v);                          // location 1 = gradient (what we want to see)
			normalOut = texture(normalIn, uv).xyz;
			uvsOut = texture(uvsIn, uv).xyz;
		}
		return;
	}

	vec2 ndc = CalcNDC();
	mat4 camToWorld = inverse(worldMatrix);
	vec4 nearPos4 = camToWorld * vec4(ndc, -1.0, 1.0);
	vec4 farPos4 = camToWorld * vec4(ndc, 1.0, 1.0);
	vec3 rayOrigin = nearPos4.xyz / nearPos4.w;
	vec3 rayDirection = normalize(farPos4.xyz / farPos4.w - rayOrigin);

	// outputToTarget=1: full res (gl_FragCoord = pixel). outputToTarget=0: coarse res (gl_FragCoord = coarse pixel).
	int tileX = (outputToTarget != 0)
		? int(gl_FragCoord.x) / tileSize
		: int(gl_FragCoord.x * float(coarseScale)) / tileSize;
	int tileY = (outputToTarget != 0)
		? int(gl_FragCoord.y) / tileSize
		: int(gl_FragCoord.y * float(coarseScale)) / tileSize;
	tileX = clamp(tileX, 0, tilesX - 1);
	tileY = clamp(tileY, 0, tilesY - 1);
	int tileIdx = tileY * tilesX + tileX;

	int tileCount = int(texelFetch(tileIndicesBuffer, tileIdx * tileStride).r);
	if (coarseDebugMode == 1) {
		vec3 color = (tileCount > 0) ? vec3(0.0, 1.0, 0.0) : vec3(0.0, 0.0, 0.0);  // green = has SDFs, black = empty
		fragColor = vec4(color, 1.0);
		if (outputToTarget != 0) {
			vec2 uv = gl_FragCoord.xy / screenSize;
			fragColor = vec4(texture(worldPosIn, uv).xyz, 1.0);
			diffuseOut = color;
			normalOut = texture(normalIn, uv).xyz;
			uvsOut = texture(uvsIn, uv).xyz;
		} else {
			diffuseOut = vec3(0.0);
			normalOut = vec3(0.0);
			uvsOut = vec3(0.0);
		}
		return;
	}
	if (tileCount <= 0) {
		float v = 0.0;
		fragColor = vec4(v, v, v, 1.0);
		if (outputToTarget != 0) {
			vec2 uv = gl_FragCoord.xy / screenSize;
			fragColor = vec4(texture(worldPosIn, uv).xyz, 1.0);
			diffuseOut = vec3(v, v, v);
			normalOut = texture(normalIn, uv).xyz;
			uvsOut = texture(uvsIn, uv).xyz;
		} else {
			diffuseOut = vec3(0.0);
			normalOut = vec3(0.0);
			uvsOut = vec3(0.0);
		}
		return;
	}

	const float maxDist = 500.0;
	bool rayHits = rayMayHitSDFForTile(tileIdx, rayOrigin, rayDirection, maxDist);
	float v = rayHits ? 1.0 : 0.0;
	fragColor = vec4(v, v, v, 1.0);
	if (outputToTarget != 0) {
		vec2 uv = gl_FragCoord.xy / screenSize;
		fragColor = vec4(texture(worldPosIn, uv).xyz, 1.0);
		diffuseOut = vec3(v, v, v);
		normalOut = texture(normalIn, uv).xyz;
		uvsOut = texture(uvsIn, uv).xyz;
	} else {
		diffuseOut = vec3(0.0);
		normalOut = vec3(0.0);
		uvsOut = vec3(0.0);
	}
}
