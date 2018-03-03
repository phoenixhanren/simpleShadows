#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec4 frag_pos;
//smooth out float depth;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
    //depth = gl_Position.z / gl_Position.w;
    frag_pos = gl_Position;
}