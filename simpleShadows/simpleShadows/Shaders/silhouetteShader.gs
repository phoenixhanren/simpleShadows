#version 330 core

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 18) out;
//layout (line_strip, max_vertices = 12) out;

in vec3 worldPos[];

uniform vec3 lightPos;
uniform vec3 finalLightPos;

float EPSILON = 0.00001;

//vec3 lightDirt = vec3(-0.2f, -1.0f, -0.3f);
void EmitLine(int startIndex, int endIndex)
{
    gl_Position = gl_in[startIndex].gl_Position;
    EmitVertex();

    gl_Position = gl_in[endIndex].gl_Position;
    EmitVertex();

    EndPrimitive();
}

void EmitLines(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    vec3 dir = normalize(gl_in[index].gl_Position.xyz -  finalLightPos);
    gl_Position = vec4(dir, 0.0f);
    EmitVertex();
    
    EndPrimitive();
}

void EmitQuad(int startIndex, int endIndex)
{
    vec3 startDir = normalize(gl_in[startIndex].gl_Position.xyz - finalLightPos);
    vec3 endDir = normalize(gl_in[endIndex].gl_Position.xyz - finalLightPos);
    gl_Position = vec4(gl_in[startIndex].gl_Position.xyz + EPSILON * startDir, gl_in[startIndex].gl_Position.w);
    EmitVertex();
    //gl_Position = gl_in[endIndex].gl_Position;
    gl_Position = vec4(gl_in[endIndex].gl_Position.xyz + EPSILON * endDir, gl_in[endIndex].gl_Position.w);
    EmitVertex();
    gl_Position = vec4(startDir, 0.0f);
    EmitVertex();
    gl_Position = vec4(endDir, 0.0f);
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
    vec3 normal = normalize(cross(e1, e2));
    vec3 lightDir = normalize(lightPos - worldPos[0]);

    if (dot(normal, lightDir) > 0)
    {
        normal = normalize(cross(e3, e1));

        if (dot(normal, lightDir) <= 0.0)
        {
            //EmitLine(0, 2);
            //EmitLines(0);
            //EmitLines(2);

            EmitQuad(0, 2);
        }

        normal = normalize(cross(e4, e5));
        lightDir = normalize(lightPos - worldPos[2]);

        if (dot(normal, lightDir) <= 0.0)
        {
            //EmitLine(2, 4);
            //EmitLines(2);
            //EmitLines(4);

            EmitQuad(2, 4);
        }

        normal = normalize(cross(e2, e6));
        lightDir = normalize(lightPos - worldPos[4]);     

        if (dot(normal, lightDir) <= 0.0)
        {
            //EmitLine(4, 0);
            //EmitLines(0);
            //EmitLines(4);

            EmitQuad(4, 0);
        }
        lightDir = normalize(gl_in[0].gl_Position.xyz - finalLightPos);
        gl_Position = vec4(gl_in[0].gl_Position.xyz + lightDir * EPSILON, gl_in[0].gl_Position.w);
        EmitVertex();

        lightDir = normalize(gl_in[2].gl_Position.xyz - finalLightPos);
        gl_Position = vec4(gl_in[2].gl_Position.xyz + lightDir * EPSILON, gl_in[2].gl_Position.w);
        EmitVertex();

        lightDir = normalize(gl_in[4].gl_Position.xyz - finalLightPos);
        gl_Position = vec4(gl_in[4].gl_Position.xyz + lightDir * EPSILON, gl_in[4].gl_Position.w);
        EmitVertex();
        EndPrimitive();

        lightDir = normalize(gl_in[0].gl_Position.xyz - finalLightPos);
        gl_Position = vec4(lightDir, 0.0f);
        EmitVertex();

        lightDir = normalize(gl_in[4].gl_Position.xyz - finalLightPos);
        gl_Position = vec4(lightDir, 0.0f);
        EmitVertex();

        lightDir = normalize(gl_in[2].gl_Position.xyz - finalLightPos);
        gl_Position = vec4(lightDir, 0.0f);
        EmitVertex();
        EndPrimitive();   

    }
    EndPrimitive();
}