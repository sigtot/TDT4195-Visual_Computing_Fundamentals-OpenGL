#version 450 core

in vec4 gl_FragCoord;
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
    vec3 rgb = hsv2rgb(vec3(gl_FragCoord.x / 1000, 1, 1));
    color = vec4(rgb, 1.0f);
}
