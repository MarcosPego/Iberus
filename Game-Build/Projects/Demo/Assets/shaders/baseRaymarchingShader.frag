#version 460 core

in mat4 worldMatrix;

uniform sampler2D worldPosIn;   
uniform sampler2D diffuseIn;     
uniform sampler2D normalIn;     
uniform sampler2D uvsIn;
uniform sampler2D depthIn;

const int maxCreatures = 256;
const int poolBudget = 4096;

struct SDFPart {
	vec4 center;   // .xyz = position, .w unused
	vec4 color;
	float radius;
	int type;
	uint blendGroupMask;  // Blend when (maskA & maskB) != 0
	vec4 endpoint;       // .xyz = capsule end, .w unused
	vec4 volumeTexture;  // Placeholder for future volume SDF
};

struct CreatureHeader {
	int partOffset;
	int partCount;
	vec4 boundCenter;  // .xyz = center, .w unused
	float boundRadius;
	int _pad;
};

// std430 SSBO - must match SDFSSBO in OpenGLRaymarchingPass.h
layout(std430, binding = 0) buffer SDFBlock {
	int creatureCount;
	int totalPartCount;
	int _pad[2];
	CreatureHeader headers[maxCreatures];
	SDFPart parts[poolBudget];
};

layout (location = 0) out vec3 worldPosOut;   
layout (location = 1) out vec3 diffuseOut;     
layout (location = 2) out vec3 normalOut;     
layout (location = 3) out vec3 uvsOut;


uniform vec2 screenSize;
uniform vec3 cameraPos;
uniform mat4 cameraToWorld;
uniform int debugRayMode;  // 0=normal, 1-4=old, 5=creatureCount, 6=tileCount, 7=rayMayHit, 8=distField, 9=raymarch hit, 10=coarseMask, 11=coarse culling, 12=raw coarse
uniform float pixelConeWidth;  // Pixel footprint at distance 1; for pixel-aware convergence
uniform float coneTraceA;     // Cone trace factor A = C/(C-aperture), C = sqrt(aperture^2+1)

uniform usamplerBuffer tileIndicesBuffer;
uniform int tilesX;
uniform int tilesY;
uniform int tileSize;
uniform int useCoarsePass;  // 1 when coarse mask is bound
uniform sampler2D coarseMask;
uniform vec2 coarseTexSize;  // coarse mask texture dimensions (width, height)
uniform int coarseDebugConstant;  // 1 = output red in coarseMask debug (pipeline test)
const int tileStride = 65;  // count + 64 indices
const int coarseScale = 8;

vec2 CalcUVCoord() {
  return gl_FragCoord.xy / screenSize;
}

vec2 CalcNDC() {
  return (gl_FragCoord.xy - 0.5) / screenSize * 2.0 - 1.0;
}

/// Ray-sphere intersection: returns true if ray hits sphere in [0, maxT]. D must be normalized.
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
	// Ray hits sphere if either t0 or t1 is in [0, maxT]
	if (t1 < 0.0 || t0 > maxT) {
		return false;
	}
	return true;
}

/// Quick test: does ray enter any SDF bound in this tile? If not, skip raymarching.
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

float distanceFieldForTile(vec3 position, int tileIdx) {
	float result = 1e30;
	int count = int(texelFetch(tileIndicesBuffer, tileIdx * tileStride).r);
	for (int k = 0; k < count; k++) {
		int i = int(texelFetch(tileIndicesBuffer, tileIdx * tileStride + 1 + k).r);
		if (i >= creatureCount || headers[i].partCount <= 0) {
			continue;
		}
		// Bounding sphere culling: if we're farther from this mesh than current best hit, skip it
		float dToBound = length(position - headers[i].boundCenter.xyz) - headers[i].boundRadius;
		if (dToBound > result) {
			continue;
		}
		int partOffset = headers[i].partOffset;
		int meshSize = headers[i].partCount;
		float resultingT = result;
		uint resultMask = 0u;
		for (int j = 0; j < meshSize; j++) {
			SDFPart part = parts[partOffset + j];
			int ptype = part.type;
			if (ptype <= 0) {
				ptype = 1;
			}
			vec3 center = part.center.xyz;
			float radius = part.radius;  // Already scaled by sdfRadiusScale in SDFRenderSystem

			float dist;
			if (ptype == 1) {
				dist = sdSphere(position - center, radius);
			} else if (ptype == 2) {
				dist = sdBox(position - center, radius);
			} else if (ptype == 3) {
				dist = sdCapsule(position, center, part.endpoint.xyz, radius);
			} else {
				dist = sdSphere(position - center, radius);
			}

			uint ma = part.blendGroupMask == 0u ? 1u : part.blendGroupMask;
			uint rm = resultMask == 0u ? 1u : resultMask;
			bool shouldBlend = (ma & rm) != 0u;

			if (j == 0) {
				resultingT = dist;
				resultMask = ma;
			} else {
				if (shouldBlend) {
					// Sharp union: no smooth blend bulging (was k=0.25)
					resultingT = min(dist, resultingT);
					resultMask = rm | ma;
				} else {
					bool partWins = dist <= resultingT;
					resultingT = min(dist, resultingT);
					resultMask = partWins ? ma : rm;
				}
			}
			if (resultingT < result) {
				float prevResult = result;
				result = resultingT;
				finalColor = (j == 0) ? part.color : mix(finalColor, part.color, clamp(prevResult - resultingT, 0.0, 1.0));
			}
		}
	}
	// Fallback disabled: bound sphere was too large (chunky); use part SDF only
	return result;
}

// 3-tap forward difference gradient - 3 evals (cheaper than 4-tap tetrahedron)
vec3 getNormalForTile(vec3 p, int tileIdx) {
	const float e = 0.001;
	float fx = distanceFieldForTile(p + vec3(e, 0, 0), tileIdx);
	float fy = distanceFieldForTile(p + vec3(0, e, 0), tileIdx);
	float fz = distanceFieldForTile(p + vec3(0, 0, e), tileIdx);
	return normalize(vec3(fx, fy, fz));
}

// Lazy normal: use -rayDir when safe (distant hits, head-on); full gradient when close or grazing
vec3 getNormalForTileLazy(vec3 p, vec3 rayDir, float hitT, int tileIdx) {
	// Far hits: normal error spreads over many pixels; -rayDir is cheap and often acceptable
	const float farThreshold = 15.0;
	if (hitT > farThreshold) {
		return -rayDir;
	}
	return getNormalForTile(p, tileIdx);
}

float raymarchingForTile(vec3 origin, vec3 direction, int tileIdx) {
	float t = 0;
	float dPrev = 1e30;
	const int maxIteration = 32;
	float maxDistance = 500.0;
	const float minEpsilon = 0.001;

	for (int i = 0; i < maxIteration; i++) {
		if (t > maxDistance) {
			return -1.0;
		}

		vec3 position = origin + direction * t;
		float d = distanceFieldForTile(position, tileIdx);

		// Pixel-aware convergence: break when surface inside pixel's bounding cone at distance t
		float coneEpsilon = max(pixelConeWidth * t, minEpsilon);
		if (d < coneEpsilon) {
			return t;
		}

		// Analytic last step: when near surface, use geometric series to reach in one step
		if (dPrev < 1e29 && d < 0.5 && abs(d - dPrev) > 1e-6) {
			float denom = 1.0 - (d - dPrev);
			if (abs(denom) > 0.01) {
				float analyticStep = d / denom;
				if (analyticStep > 0.0 && analyticStep < 1.0) {
					t += analyticStep;
					return t;
				}
			}
		}

		if (d < 0.0) {
			t += max(-d * 0.5, minEpsilon);
		} else {
			// Cone tracing: t = (t + D) * A allows larger steps in empty space (Claybook)
			float sphereStep = max(d, minEpsilon);
			if (coneTraceA > 1.0) {
				t = (t + sphereStep) * coneTraceA;
			} else {
				t += sphereStep;
			}
		}

		dPrev = d;
	}

	return -1.0;
}

/// 

void main(void)
{
	vec2 uvCoord = CalcUVCoord();

	int tileX = int(gl_FragCoord.x) / tileSize;
	int tileY = int(gl_FragCoord.y) / tileSize;
	tileX = clamp(tileX, 0, tilesX - 1);
	tileY = clamp(tileY, 0, tilesY - 1);
	int tileIdx = tileY * tilesX + tileX;

	// Debug 5: SSBO creatureCount (before any early-out)
	if (debugRayMode == 5) {
		vec3 dbg = (creatureCount > 0) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
		worldPosOut = texture(worldPosIn, uvCoord).xyz;
		diffuseOut = dbg;
		normalOut = texture(normalIn, uvCoord).xyz;
		uvsOut = texture(uvsIn, uvCoord).xyz;
		gl_FragDepth = texture(depthIn, uvCoord).r;
		return;
	}

	// Coarse pre-pass: skip full raymarch if 8x8 block had no hit
	// Constant red test (coarseDebugConstant): output red BEFORE sampling coarse - isolates pipeline from coarse texture bugs
	if (useCoarsePass != 0 && debugRayMode == 10 && coarseDebugConstant != 0) {
		worldPosOut = texture(worldPosIn, uvCoord).xyz;
		diffuseOut = vec3(1.0, 0.0, 0.0);
		normalOut = texture(normalIn, uvCoord).xyz;
		uvsOut = texture(uvsIn, uvCoord).xyz;
		gl_FragDepth = texture(depthIn, uvCoord).r;
		return;
	}
	if (useCoarsePass != 0) {
		ivec2 blockCoord = ivec2(floor(gl_FragCoord.xy / float(coarseScale)));
		blockCoord = clamp(blockCoord, ivec2(0), ivec2(coarseTexSize) - 1);
		float coarseHit = texelFetch(coarseMask, blockCoord, 0).r;
		// Debug 10: visualize coarse mask (grayscale)
		if (debugRayMode == 10) {
			worldPosOut = texture(worldPosIn, uvCoord).xyz;
			diffuseOut = vec3(coarseHit, coarseHit, coarseHit);
			normalOut = texture(normalIn, uvCoord).xyz;
			uvsOut = texture(uvsIn, uvCoord).xyz;
			gl_FragDepth = texture(depthIn, uvCoord).r;
			return;
		}
		// Debug 11: coarse culling - red=culled (no raymarch), green=raymarched
		if (debugRayMode == 11) {
			vec3 dbg = (coarseHit >= 0.5) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
			worldPosOut = texture(worldPosIn, uvCoord).xyz;
			diffuseOut = dbg;
			normalOut = texture(normalIn, uvCoord).xyz;
			uvsOut = texture(uvsIn, uvCoord).xyz;
			gl_FragDepth = texture(depthIn, uvCoord).r;
			return;
		}
		// Debug 12: raw coarse texture - grayscale, no culling logic (pipeline test)
		if (debugRayMode == 12) {
			worldPosOut = texture(worldPosIn, uvCoord).xyz;
			diffuseOut = vec3(coarseHit, coarseHit, coarseHit);
			normalOut = texture(normalIn, uvCoord).xyz;
			uvsOut = texture(uvsIn, uvCoord).xyz;
			gl_FragDepth = texture(depthIn, uvCoord).r;
			return;
		}
		if (coarseHit < 0.5) {
			worldPosOut = texture(worldPosIn, uvCoord).xyz;
			diffuseOut = texture(diffuseIn, uvCoord).xyz;
			normalOut = texture(normalIn, uvCoord).xyz;
			uvsOut = texture(uvsIn, uvCoord).xyz;
			gl_FragDepth = texture(depthIn, uvCoord).r;
			return;
		}
	}

	// Early-out: no SDF entities in this tile - use geometry directly
	int tileCount = int(texelFetch(tileIndicesBuffer, tileIdx * tileStride).r);
	if (tileCount <= 0) {
		vec3 dbgOut = (debugRayMode == 6) ? vec3(1.0, 0.0, 0.0) : texture(diffuseIn, uvCoord).xyz;
		worldPosOut = texture(worldPosIn, uvCoord).xyz;
		diffuseOut = dbgOut;
		normalOut = texture(normalIn, uvCoord).xyz;
		uvsOut = texture(uvsIn, uvCoord).xyz;
		gl_FragDepth = texture(depthIn, uvCoord).r;
		return;
	}
	if (debugRayMode == 6) {
		worldPosOut = texture(worldPosIn, uvCoord).xyz;
		diffuseOut = vec3(0.0, 1.0, 0.0);  // tile has SDFs
		normalOut = texture(normalIn, uvCoord).xyz;
		uvsOut = texture(uvsIn, uvCoord).xyz;
		gl_FragDepth = texture(depthIn, uvCoord).r;
		return;
	}

	vec2 ndc = CalcNDC();
	mat4 camToWorld = inverse(worldMatrix);

	vec4 nearPos4 = camToWorld * vec4(ndc, -1.0, 1.0);
	vec4 farPos4 = camToWorld * vec4(ndc, 1.0, 1.0);

	vec3 nearPos = nearPos4.xyz / nearPos4.w;
	vec3 farPos = farPos4.xyz / farPos4.w;

	vec3 rayOrigin = nearPos;
	vec3 rayDirection = normalize(farPos - nearPos);

	// Ray-bounds early skip: if ray can't hit any SDF in this tile, use geometry
	const float maxDist = 500.0;
	bool rayMayHit = rayMayHitSDFForTile(tileIdx, rayOrigin, rayDirection, maxDist);
	if (!rayMayHit) {
		vec3 dbgOut = (debugRayMode == 7) ? vec3(1.0, 0.0, 0.0) : texture(diffuseIn, uvCoord).xyz;
		worldPosOut = texture(worldPosIn, uvCoord).xyz;
		diffuseOut = dbgOut;
		normalOut = texture(normalIn, uvCoord).xyz;
		uvsOut = texture(uvsIn, uvCoord).xyz;
		gl_FragDepth = texture(depthIn, uvCoord).r;
		return;
	}
	if (debugRayMode == 7) {
		worldPosOut = texture(worldPosIn, uvCoord).xyz;
		diffuseOut = vec3(0.0, 1.0, 0.0);  // ray passes bounds test
		normalOut = texture(normalIn, uvCoord).xyz;
		uvsOut = texture(uvsIn, uvCoord).xyz;
		gl_FragDepth = texture(depthIn, uvCoord).r;
		return;
	}

	vec3 normal = vec3(0,0,0);
	vec3 pos = vec3(0,0,0);

	float t = raymarchingForTile(rayOrigin, rayDirection, tileIdx);
	if (t >= 0.0) {
		pos = rayOrigin + rayDirection * t;
		normal = getNormalForTileLazy(pos, rayDirection, t, tileIdx);
	} else {
		finalColor = vec4(0, 0, 0, 0);
	}

	// Depth occlusion: if geometry is closer than SDF hit, use geometry
	if (t >= 0.0) {
		float geomDepth = texture(depthIn, uvCoord).r;
		vec4 clipPos = worldMatrix * vec4(pos, 1.0);
		float raymarchDepth = (clipPos.z / clipPos.w) * 0.5 + 0.5;
		const float depthBias = 0.0001;
		if (raymarchDepth > geomDepth + depthBias) {
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

	vec3 outColor;
	if (debugRayMode == 1) {
		outColor = 0.5 + 0.5 * rayDirection;
	} else if (debugRayMode == 2) {
		outColor = (t >= 0.0) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	} else if (debugRayMode == 3) {
		float d = distanceFieldForTile(rayOrigin, tileIdx);
		outColor = vec3(1.0 - smoothstep(0.0, 50.0, d));
	} else if (debugRayMode == 4) {
		outColor = vec3(ndc * 0.5 + 0.5, 0.0);
	} else if (debugRayMode == 5) {
		// Debug: SSBO creatureCount - green if >0 (reading buffer), red if 0
		outColor = (creatureCount > 0) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	} else if (debugRayMode == 6) {
		// Debug: tile buffer - green if tile has SDFs, red if empty
		outColor = (tileCount > 0) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	} else if (debugRayMode == 7) {
		// Debug: rayMayHit (would need to recompute, use tileCount as proxy - tiles with SDFs)
		bool mayHit = rayMayHitSDFForTile(tileIdx, rayOrigin, rayDirection, 500.0);
		outColor = mayHit ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	} else if (debugRayMode == 8) {
		// Debug: distance field at ray origin - dark=far, bright=near
		float d = distanceFieldForTile(rayOrigin, tileIdx);
		outColor = vec3(1.0 - smoothstep(0.0, 20.0, d));
	} else if (debugRayMode == 9) {
		// Debug: raymarch result - green=hit, red=miss
		outColor = (t >= 0.0) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
	} else if (debugRayMode == 10) {
		outColor = vec3(0.0, 0.0, 0.0);  // coarseMask: only used when useCoarsePass, handled above
	} else {
		outColor = sdfInFront ? finalColor.xyz : geomDiffuse;
	}

	worldPosOut     = sdfInFront ? pos : geomPos;
	diffuseOut      = outColor;
	normalOut       = sdfInFront ? normal : geomNormal;
	// SDF has no emissive; only pass through geometry's emissive when showing geometry (avoids adding occluded geometry's emissive to SDF = see-through).
	uvsOut			= sdfInFront ? vec3(0.0) : texture(uvsIn, uvCoord).xyz;

	// Conservative depth write for early-Z in later passes
	if (sdfInFront) {
		vec4 clipPos = worldMatrix * vec4(pos, 1.0);
		gl_FragDepth = (clipPos.z / clipPos.w) * 0.5 + 0.5;
	} else {
		gl_FragDepth = texture(depthIn, uvCoord).r;
	}
}
