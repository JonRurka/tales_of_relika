#version 450 core

layout (location = 0) out vec4 FragColor;
  
layout (location = 3) in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, TexCoords);
}