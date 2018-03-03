#version 330 core

//smooth in float depth;
in vec4 frag_pos;
layout (location = 0) out vec3 fragColor;

void main()
{
    float depth = frag_pos.z / frag_pos.w;
    depth = depth * 0.5 + 0.5;

    float m1 = depth;
    float m2 = depth * depth;

    fragColor = vec3(m1, m2, 0.0);
}