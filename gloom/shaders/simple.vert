#version 450 core

in vec3 position;

void main()
{
    vec3 mirror_position = vec3(-position.x, -position.y, position.z);
    gl_Position = vec4(mirror_position, 1.0f);
}
