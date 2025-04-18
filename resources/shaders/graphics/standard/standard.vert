#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

layout (location = 4) out vec3 Normal;
layout (location = 5) out vec3 Color;
layout (location = 6) out vec2 TexCoords;
layout (location = 7) out vec3 FragPos; 

layout (location = 8) uniform mat3 normal_mat;
layout (location = 9) uniform mat4 model_mat;
layout (location = 10) uniform mat4 view_mat;
layout (location = 11) uniform mat4 projection_mat;

void main()
{
    gl_Position = projection_mat * view_mat * model_mat * vec4(aPos, 1.0);
	FragPos = vec3(model_mat * vec4(aPos, 1.0));
	Normal = normal_mat * aNormal;
    Color = aColor;
    TexCoords = aTexCoord;
}
