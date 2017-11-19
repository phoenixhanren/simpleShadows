#version 330 core

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 18) out;

in vec3 worldPos[];
in vec3 lightWorldPos;
in mat4 v;
in mat4 p;
in mat4 m;

float EPSILON = 0.0001;

void EmitQuad2(int startIndex, int endIndex)
{
    vec3 startVertex = gl_in[startIndex].gl_Position.xyz;
    vec3 endVertex = gl_in[endIndex].gl_Position.xyz;
    vec3 lightDir = normalize(startVertex - lightWorldPos);
    gl_Position = p * v * vec4((startVertex + lightDir * EPSILON), 1.0);
    EmitVertex();

    gl_Position = p * v * vec4(lightDir, 0.0f);
    EmitVertex();

    EndPrimitive();
    lightDir = normalize(endVertex - lightWorldPos);
    gl_Position = p * v * vec4((endVertex + lightDir * EPSILON), 1.0);
    EmitVertex();

    gl_Position = p * v * vec4(lightDir, 0.0f);
    EmitVertex();

    EndPrimitive();
}

void EmitQuad(vec3 startVertex, vec3 endVertex)
{
    vec3 lightDir = normalize(startVertex - lightWorldPos);
    gl_Position = p * v * vec4((startVertex + lightDir * EPSILON), 1.0);
    EmitVertex();

    gl_Position = p * v * vec4(lightDir, 0.0f);
    EmitVertex();

    lightDir = normalize(endVertex - lightWorldPos);
    gl_Position = p * v * vec4((endVertex + lightDir * EPSILON), 1.0);
    EmitVertex();

    gl_Position = p * v * vec4(lightDir, 0.0f);
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

    vec3 normal = normalize(cross(e1, e2));
    vec3 lightDir = normalize(lightWorldPos - worldPos[0]);

    if (dot(normal, lightDir) > 0)
    {
        normal = normalize(cross(e3, e1));

        if (dot(normal, lightDir) <= 0.0)
        {
            vec3 startVertex = worldPos[0];
            vec3 endVertex = worldPos[2];
            //EmitQuad(startVertex, endVertex);
            EmitQuad2(0, 2);
        }

        normal = cross(e4, e5);
        lightDir = normalize(lightWorldPos - worldPos[2]);

        if (dot(normal, lightDir) <= 0.0)
        {
            vec3 startVertex = worldPos[2];
            vec3 endVertex = worldPos[4];
            //EmitQuad(startVertex, endVertex);
            EmitQuad2(2, 4);
        }

        normal = cross(e2, e6);
        lightDir = normalize(lightWorldPos - worldPos[4]);

        if (dot(normal, lightDir) <= 0.0)
        {
            vec3 startVertex = worldPos[4];
            vec3 endVertex = worldPos[0];
            //EmitQuad(startVertex, endVertex);
            EmitQuad2(4, 0);
        }

        //lightDir = (normalize(worldPos[0] - lightWorldPos));
       /// gl_Position = p * v  * vec4((worldPos[0] + lightDir * EPSILON), 1.0f);
        //EmitVertex();

        //lightDir = (normalize(worldPos[2] - lightWorldPos));
        //gl_Position = p * v * vec4((worldPos[2] + lightDir * EPSILON), 1.0f);
        //EmitVertex();
        
        //lightDir = (normalize(worldPos[4] - lightWorldPos));
        //gl_Position = p * v  * vec4((worldPos[4] + lightDir * EPSILON), 1.0f);
        //EmitVertex();

       // EndPrimitive();

        //lightDir = (normalize(worldPos[0] - lightWorldPos));
        //gl_Position = p * v * vec4(lightDir , 0.0f);
        //EmitVertex();

        //lightDir = (normalize(worldPos[4] - lightWorldPos));
        //gl_Position = p * v * vec4(lightDir , 0.0f);
        //EmitVertex();
        
        //lightDir = (normalize(worldPos[2] - lightWorldPos));
       // gl_Position = p * v  * vec4(lightDir , 0.0f);
        //EmitVertex();

        //EndPrimitive();    
    }

}
