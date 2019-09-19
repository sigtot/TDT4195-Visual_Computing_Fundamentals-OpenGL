#version 450 core
#define TIME_STEP 0.01f
#define FPS 60
#define PERIOD 3
#define PI 3.1415

in vec3 position;
in vec4 color;
uniform int frame_num;

out vec4 ex_color;
void main()
{
    float t = float(frame_num) * PI/(PERIOD * FPS);
    float a = (frame_num / (PERIOD*FPS)) % 6 == 0 ? sin(t) : 1.0;
    float b = (frame_num / (PERIOD*FPS)) % 6 == 1 ? sin(t) : 0.0;
    float c = (frame_num / (PERIOD*FPS)) % 6 == 2 ? sin(t) : 0.0;
    float d = (frame_num / (PERIOD*FPS)) % 6 == 3 ? sin(t) : 0.0;
    float e = (frame_num / (PERIOD*FPS)) % 6 == 4 ? sin(t) : 1.0;
    float f = (frame_num / (PERIOD*FPS)) % 6 == 5 ? sin(t) : 0.0;
    mat4 transition_mat = mat4(a,   d,   0.0, 0.0,
                               b,   e,   0.0, 0.0,
                               0.0, 0.0, 1.0, 0.0,
                               c,   f,   0.0, 1.0);
    vec4 new_pos = transition_mat*vec4(position, 1.0);
    gl_Position = new_pos;
    ex_color = color;
}
