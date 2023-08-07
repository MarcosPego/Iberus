#version 330 core

in vec3 exVerticies;
in vec2 exUVs;
in vec3 exNormals;

out vec4 FragmentColor;

void main(void)
{
	vec3 color = (exNormals + vec3(1.0)) * 0.5;
	FragmentColor = vec4(vec3(1.0),1.0);
}
