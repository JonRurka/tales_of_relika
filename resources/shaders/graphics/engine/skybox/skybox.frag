#version 450 core

layout (location = 0) out vec4 FragColor;

layout (location = 3) in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}
