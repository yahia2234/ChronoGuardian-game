#version 120

attribute vec3 aPos;
attribute vec4 aColor;
attribute float aSize;

varying vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    ParticleColor = aColor;
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = aSize;
}
