#version 330

uniform mat4 u_worldViewProj;

in vec4 position;

void main(void)
{
    gl_Position = u_worldViewProj * position;
}
