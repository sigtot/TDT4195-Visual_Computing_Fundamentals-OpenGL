#version 450 core

in vec3 position;
in vec4 color;

out vec4 ex_color;
void main()
{
    gl_Position = vec4(position, 1.0f);
    ex_color = color;
}
