#version 460 core

// Depth+normal-based outline (shadows) and highlight. Based on godotshaders 3D pixel art outline & highlight.
// Uses worldPos for linear depth; normals for edge detection.

uniform sampler2D worldPosIn;
uniform sampler2D diffuseIn;
uniform sampler2D normalIn;
uniform sampler2D uvsIn;

uniform vec2 screenSize;
uniform vec3 cameraPos = vec3(0.0, 0.0, 0.0);

uniform float shadowStrength = 0.0;
uniform float highlightStrength = 0.0;
uniform vec3 shadowColor = vec3(0.0, 0.0, 0.0);
uniform vec3 highlightColor = vec3(1.0, 1.0, 1.0);

layout(location = 0) out vec3 worldPosOut;
layout(location = 1) out vec3 diffuseOut;
layout(location = 2) out vec3 normalOut;
layout(location = 3) out vec3 uvsOut;

vec2 CalcUVCoord() {
	return gl_FragCoord.xy / screenSize;
}

float LinearDepth(vec3 worldPos) {
	return length(worldPos - cameraPos);
}

float NormalIndicator(vec3 baseN, vec3 newN, vec3 bias, float dDiff) {
	float nDiff = dot(baseN - newN, bias);
	float nInd = clamp(smoothstep(-0.01, 0.01, nDiff), 0.0, 1.0);
	float dInd = clamp(sign(dDiff * 0.25 + 0.0025), 0.0, 1.0);
	return (1.0 - dot(baseN, newN)) * dInd * nInd;
}

void main(void)
{
	vec2 uvCoord = CalcUVCoord();
	vec2 texelSize = 1.0 / screenSize;

	vec3 worldPos = texture(worldPosIn, uvCoord).xyz;
	vec3 baseColor = texture(diffuseIn, uvCoord).xyz;
	vec3 normal = texture(normalIn, uvCoord).xyz;
	vec3 uvs = texture(uvsIn, uvCoord).xyz;

	// Pass-through when both effects disabled
	if (shadowStrength <= 0.0 && highlightStrength <= 0.0) {
		worldPosOut = worldPos;
		diffuseOut = baseColor;
		normalOut = normal;
		uvsOut = uvs;
		return;
	}

	// Skip outline for background/sky pixels (no geometry)
	if (dot(normal, normal) < 0.001) {
		worldPosOut = worldPos;
		diffuseOut = baseColor;
		normalOut = normal;
		uvsOut = uvs;
		return;
	}

	float depth = LinearDepth(worldPos);

	// Depth-based shadow mask: neighbor farther => we're on front-facing edge
	float depthDiff = 0.0;
	float negDepthDiff = 0.5;
	if (shadowStrength > 0.0) {
		vec3 wpU = texture(worldPosIn, uvCoord + vec2(0.0, -1.0) * texelSize).xyz;
		vec3 wpR = texture(worldPosIn, uvCoord + vec2(1.0, 0.0) * texelSize).xyz;
		vec3 wpD = texture(worldPosIn, uvCoord + vec2(0.0, 1.0) * texelSize).xyz;
		vec3 wpL = texture(worldPosIn, uvCoord + vec2(-1.0, 0.0) * texelSize).xyz;

		float dU = LinearDepth(wpU);
		float dR = LinearDepth(wpR);
		float dD = LinearDepth(wpD);
		float dL = LinearDepth(wpL);

		depthDiff += clamp(dU - depth, 0.0, 1.0);
		depthDiff += clamp(dD - depth, 0.0, 1.0);
		depthDiff += clamp(dR - depth, 0.0, 1.0);
		depthDiff += clamp(dL - depth, 0.0, 1.0);

		negDepthDiff += depth - dU;
		negDepthDiff += depth - dD;
		negDepthDiff += depth - dR;
		negDepthDiff += depth - dL;
		negDepthDiff = clamp(negDepthDiff, 0.0, 1.0);
		negDepthDiff = clamp(smoothstep(0.5, 0.5, negDepthDiff) * 10.0, 0.0, 1.0);
		depthDiff = smoothstep(0.2, 0.3, depthDiff);
	}

	// Normal-based highlight mask: normals differ at edges
	float normalDiff = 0.0;
	if (highlightStrength > 0.0) {
		vec3 nU = texture(normalIn, uvCoord + vec2(0.0, -1.0) * texelSize).xyz;
		vec3 nR = texture(normalIn, uvCoord + vec2(1.0, 0.0) * texelSize).xyz;
		vec3 nD = texture(normalIn, uvCoord + vec2(0.0, 1.0) * texelSize).xyz;
		vec3 nL = texture(normalIn, uvCoord + vec2(-1.0, 0.0) * texelSize).xyz;

		vec3 normalEdgeBias = vec3(1.0, 1.0, 1.0);

		normalDiff += NormalIndicator(normal, nU, normalEdgeBias, depthDiff);
		normalDiff += NormalIndicator(normal, nR, normalEdgeBias, depthDiff);
		normalDiff += NormalIndicator(normal, nD, normalEdgeBias, depthDiff);
		normalDiff += NormalIndicator(normal, nL, normalEdgeBias, depthDiff);

		normalDiff = smoothstep(0.2, 0.8, normalDiff);
		normalDiff = clamp(normalDiff - negDepthDiff, 0.0, 1.0);
	}

	vec3 finalHighlightColor = mix(baseColor, highlightColor, highlightStrength);
	vec3 finalShadowColor = mix(baseColor, shadowColor, shadowStrength);

	vec3 finalColor = baseColor;
	if (highlightStrength > 0.0) {
		finalColor = mix(finalColor, finalHighlightColor, normalDiff);
	}
	if (shadowStrength > 0.0) {
		finalColor = mix(finalColor, finalShadowColor, depthDiff);
	}

	worldPosOut = worldPos;
	diffuseOut = finalColor;
	normalOut = normal;
	uvsOut = uvs;
}
