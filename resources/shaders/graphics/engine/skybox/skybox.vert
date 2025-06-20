#version 450 core



#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;

layout (location = 0) out vec3 TexCoords;

layout (location = 10) uniform mat4 projection;
layout (location = 11) uniform mat4 view;

void main()
{
    TexCoords = aPos;
	vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}

