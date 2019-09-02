#version 450 core
#define CHECKER_SIZE 30

in vec4 gl_FragCoord;
out vec4 color;

// Author https://gist.github.com/yiwenl
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 checker(vec2 xy)
{
    float color = (bool(int(xy.x / CHECKER_SIZE) % 2) != (bool(int(xy.y / CHECKER_SIZE) % 2))) ? 0.0f : 1.0f;
    return vec3(color, color, color);
}

void main()
{
    //vec3 rgb = hsv2rgb(vec3(gl_FragCoord.x / 1000, 1, 1));
    vec3 rgb = checker(vec2(gl_FragCoord.x, gl_FragCoord.y));
    color = vec4(rgb, 1.0f);
}
