#version 450 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNormal;
layout (location = 2) in vec4 aMat;
//layout (location = 2) in vec3 aColor;


layout (location = 0) out vec3 FragPos; 
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 TexCoords;
layout (location = 3) out vec4 Mats;
//layout (location = 2) out vec3 Color;
//

layout (location = 0) uniform mat3 normal_mat;
layout (location = 1) uniform mat4 model_mat;
layout (location = 2) uniform mat4 view_mat;
layout (location = 3) uniform mat4 projection_mat;

void main()
{
    gl_Position = projection_mat * view_mat * model_mat * vec4(aPos.xyz, 1.0);
	FragPos = vec3(model_mat * vec4(aPos.xyz, 1.0));
	Normal = normal_mat * aNormal.xyz;
    TexCoords = vec2(aPos.w, aNormal.w);
	Mats = aMat;
}