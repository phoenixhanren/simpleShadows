#version 330 core

layout (triangles_adjacency) in;
layout (line_strip, max_vertices = 6) out;

in vec3 worldPos[];

uniform vec3 lightPos;

void EmitLine(int startIndex, int endIndex)
{
    gl_Position = gl_in[startIndex].gl_Position;
    EmitVertex();

    gl_Position = gl_in[endIndex].gl_Position;
    EmitVertex();

    EndPrimitive();
}

void main()
{
    vec3 e1 = worldPos[2] - worldPos[0];
    vec3 e2 = worldPos[4] - worldPos[0];
    vec3 e3 = worldPos[1] - worldPos[0];
    vec3 e4 = worldPos[3] - worldPos[2];
    vec3 e5 = worldPos[4] - worldPos[2];
    vec3 e6 = worldPos[5] - worldPos[0];

    //e1, e2
    vec3 normal = cross(e1, e2);
    vec3 lightDir = lightPos - worldPos[0];

    if (dot(normal, lightDir) > 0.00001)
    {
        normal = cross(e3, e1);

        if (dot(normal, lightDir) <= 0.0)
        {
            EmitLine(0, 2);
        }

        normal = cross(e4, e5);

        if (dot(normal, lightDir) <= 0.0)
        {
            EmitLine(2, 4);
        }

        normal = cross(e2, e6);

        if (dot(normal, lightDir) <= 0.0)
        {
            EmitLine(0, 4);
        }

    }

}