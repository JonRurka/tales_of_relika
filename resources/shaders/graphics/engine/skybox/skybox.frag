#version 450 core



#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 TexCoords;

layout (location = 0) out vec4 FragColor;

uniform samplerCube skybox;
//layout (binding = 0) uniform sampler2D skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
	//FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
