#version 450 core

layout (location = 0)out vec4 FragColor;

layout (location = 2)uniform vec3 color;

void main()
{
	vec3 resColor = color;
    FragColor = vec4(resColor, 1.0f);
}





