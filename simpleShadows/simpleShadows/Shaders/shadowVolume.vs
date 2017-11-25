#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

//out vec3 worldPos;

uniform mat4 model;

void main()
{
    //worldPos = vec3(model * vec4(position, 1.0f)); //translate vertex to world space
    gl_Position = model * vec4(position, 1.0f);
}