#version 330 core

uniform sampler2D worldPosIn;   
uniform sampler2D diffuseIn;     
uniform sampler2D normalIn;     
uniform sampler2D uvsIn; 

//uniform vec4 albedoColor;

//uniform sampler2D albedoTexture;
//uniform sampler2D normalTexture;
//uniform sampler2D metallicTexture;

layout (location = 0) out vec4 fragColor;    

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

uniform vec2 screenSize;

uniform vec3 cameraPos;

vec2 CalcUVCoord() {
    return gl_FragCoord.xy / screenSize;
	//  return (gl_FragCoord.xy -.5 * screenSize.xy) / screenSize.y;
}

void main(void)
{
	vec2 uvCoord = CalcUVCoord();

	vec3 position = texture(worldPosIn, uvCoord).xyz;

	vec3 lightBaseColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 lightPos = vec3(15.0f, 15.0f, 25.0f);

	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightBaseColor;

	vec3 normalizedNormal = normalize(texture(normalIn, uvCoord).xyz);
	vec3 lightDir = normalize(lightPos - position);
	float diff = max(dot(normalizedNormal, lightDir), 0.0);
	vec3 diffuse = diff * lightBaseColor;
	
	vec3 lightColor = (ambient + diffuse);

	vec3 color = texture(diffuseIn, uvCoord).xyz;
	fragColor = vec4(lightColor, 1.0) * vec4(color, 1.0);	
	//fragColor = vec4(texture(normalIn, uvCoord).xyz, 1);
}
