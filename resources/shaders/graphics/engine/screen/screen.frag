#version 450 core

#define USE_SPIRV

layout (location = 0) in vec2 TexCoords;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}