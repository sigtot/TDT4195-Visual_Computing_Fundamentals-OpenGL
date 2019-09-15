#version 450 core
#define CHECKER_SIZE 30
#define WINDOW_WIDTH 768

in vec4 ex_color;
out vec4 color;

// Author https://gist.github.com/yiwenl
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    color = ex_color;
}
