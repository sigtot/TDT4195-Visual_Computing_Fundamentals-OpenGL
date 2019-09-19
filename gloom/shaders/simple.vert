#version 450 core

in vec3 position;
in vec4 color;

out vec4 ex_color;
void main()
{
    mat4 transition_mat = mat4(1.0, 0.0, 0.0, 0.0,
                               0.0, 1.0, 0.0, 0.0,
                               0.0, 0.0, 1.0, 0.0,
                               0.0, 0.0, 0.0, 1.0);
    vec4 new_pos = transition_mat*vec4(position, 1.0);
    gl_Position = new_pos;
    ex_color = color;
}
