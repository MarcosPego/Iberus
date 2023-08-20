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
  //return (gl_FragCoord.xy -.5 * screenSize.xy) / screenSize.y;
}

/// Distance Field Functions

float sdSphere(vec3 position, float size) {
	return length(position) - size;
}

float sdBox(vec3 position, float size) {
	vec3 distance = abs(position) - size;
	return min(max(distance.x, max(distance.y, distance.z)), 0.0) + length(max(distance, 0.0));
}

float distanceField(vec3 position) {
	float Sphere1 = sdSphere(position - vec3(0, 0, 20), 1.0);
	return Sphere1;
}

vec3 getNormal(vec3 position) {
	const vec2 offset = vec2(0.001, 0.0);
	return vec3(
		-(distanceField(position + offset.xyy) - distanceField(position - offset.xyy)), // not sure why the x has to be inverted
		distanceField(position + offset.yxy) - distanceField(position - offset.yxy),
		distanceField(position + offset.yyx) - distanceField(position - offset.yyx)
	);


	/*float epsilon = 0.001; // arbitrary — should be smaller than any surface detail in your distance function, but not so small as to get lost in float precision
	float centerDistance = distanceField(position);
	float xDistance = distanceField(position + vec3(epsilon, 0, 0));
	float yDistance = distanceField(position + vec3(0, epsilon, 0));
	float zDistance = distanceField(position + vec3(0, 0, epsilon));
	return (vec3(xDistance, yDistance, zDistance) - centerDistance) / epsilon;*/

	/*const float eps = 0.001;
	  const vec3 v1 = vec3( 1.0,-1.0,-1.0);
	  const vec3 v2 = vec3(-1.0,-1.0, 1.0);
	  const vec3 v3 = vec3(-1.0, 1.0,-1.0);
	  const vec3 v4 = vec3( 1.0, 1.0, 1.0);

	  return normalize( v1 * distanceField( position + v1*eps ) +
						v2 * distanceField( position + v2*eps ) +
						v3 * distanceField( position + v3*eps ) +
						v4 * distanceField( position + v4*eps ));*/
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

	vec4 color = vec4(0,0,0,0);
	vec3 normal = vec3(0,0,0);
	vec3 pos = vec3(0,0,0);

	float t = raymarching(rayOrigin, rayDirection);
	if (t >= 0.0) {
		pos = rayOrigin + rayDirection * t;
		color = vec4(0, 0.8, 1, 1);
		//rayDirection = vec3(-rayDirection.x, rayDirection.y, rayDirection.z); // don't know why this is needed;
		normal =  normalize(getNormal(pos));
	}

	worldPosOut     = texture(worldPosIn, uvCoord).xyz + pos;				
	diffuseOut      = texture(diffuseIn, uvCoord).xyz + color.xyz;	
	normalOut       = texture(normalIn, uvCoord).xyz + normal;					
	uvsOut			= texture(uvsIn, uvCoord).xyz;	
}
