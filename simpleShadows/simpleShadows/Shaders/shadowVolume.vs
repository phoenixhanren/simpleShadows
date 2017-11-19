#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec3 worldPos;
out vec3 lightWorldPos;
out mat4 v;
out mat4 p;
out mat4 m;

uniform vec3 lightPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    p = projection;
    v = view;
    m = model;

    worldPos = vec3(model * vec4(position, 1.0f));
    lightWorldPos = lightPos;
}