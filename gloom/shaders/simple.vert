#version 450 core

in vec3 position;
in vec4 color;
in vec3 normal;
uniform mat4 t_mat;

out vec4 ex_color;
out vec3 ex_normal;
void main()
{
    vec4 new_pos = t_mat*vec4(position, 1.0);
    gl_Position = new_pos;
    ex_color = color;
    ex_normal = normal;
}
