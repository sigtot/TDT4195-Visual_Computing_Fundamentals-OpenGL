#version 450 core

in layout(location=1) vec4 ex_color;
in layout(location=2) vec3 ex_normal;
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
    vec3 lightDir = normalize(vec3(0.8, -0.5, 0.6));
    color = vec4(vec3(1,1,1) * max(0, dot(ex_normal, -lightDir)), 1.0f);
}
