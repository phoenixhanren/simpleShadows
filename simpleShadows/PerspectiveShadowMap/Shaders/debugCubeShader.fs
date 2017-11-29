#version 330 core

in vec2 TexCoords;

uniform sampler2D depthMap;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vec3(texture(depthMap, TexCoords).r), 1.0);
}