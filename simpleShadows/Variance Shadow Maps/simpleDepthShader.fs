#version 330 core

//smooth in float depth;
in vec4 frag_pos;
layout (location = 0) out vec2 fragColor;

void main()
{
   // float depth = frag_pos.z / frag_pos.w;
    
    float depth = gl_FragCoord.z;
   // depth = depth * 0.5 + 0.5;
    float m1 = depth;
    float m2 = depth * depth;

    float dx = dFdx(m1);
    float dy = dFdy(m1);
    m2 += 0.25 * (dx * dx + dy * dy); //make var more var for shadow arnce

    fragColor = vec2(m1, m2);
}