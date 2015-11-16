#version 330 core
layout (location = 0) in vec4 offset;
layout (location = 1) in vec4 normal;

uniform mat4 mvp_matrix;

void main()
{
    gl_Position = mvp_matrix * offset;
}