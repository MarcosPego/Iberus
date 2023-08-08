#version 330 core

in vec3 exVerticies;
in vec2 exUVs;
in vec3 exNormals;

uniform vec4 Color;

void main(void)
{
	vec3 color = (exNormals + vec3(1.0)) * 0.5;
	gl_FragColor = vec4(color,1.0);
}
