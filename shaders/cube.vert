#version 410 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform vec3 u_color;

out vec3 out_color;


void main()
{
    gl_Position = u_proj * u_view * u_model * vec4(aPos, 1.0);
    out_color = u_color;
}