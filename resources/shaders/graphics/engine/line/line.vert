#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 view_mat;
uniform mat4 projection_mat;

void main()
{
    gl_Position = projection_mat * view_mat * vec4(aPos, 1.0);
}
