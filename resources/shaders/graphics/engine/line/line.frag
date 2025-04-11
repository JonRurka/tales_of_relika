#version 450 core

out vec4 FragColor;

uniform vec3 color;

void main()
{
	vec3 resColor = color;
    FragColor = vec4(resColor, 1.0f);
}





