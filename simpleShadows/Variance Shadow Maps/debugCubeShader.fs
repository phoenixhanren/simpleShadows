#version 330 core

in vec2 TexCoords;

uniform bool m1;
uniform sampler2D depthMap;

out vec4 FragColor;

void main()
{
    
    FragColor = vec4(vec3(texture(depthMap, TexCoords).r), 1.0);
    if (m1 == false)
        FragColor = vec4(vec3(texture(depthMap, TexCoords).g), 1.0);
}