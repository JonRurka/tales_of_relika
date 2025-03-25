#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

out vec3 Normal;
out vec3 Color;
out vec2 TexCoords;
out vec3 FragPos; 

uniform mat3 normal_mat;
uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

void main()
{
    gl_Position = projection_mat * view_mat * model_mat * vec4(aPos, 1.0);
	FragPos = vec3(model_mat * vec4(aPos, 1.0));
	Normal = normal_mat * aNormal;
    Color = aColor;
    TexCoords = aTexCoord;
}
