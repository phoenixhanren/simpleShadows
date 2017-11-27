#version 330 core

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 18) out;

//in vec3 worldPos[];

uniform vec3 lightPos; //light position in world space
uniform mat4 view;
uniform mat4 projection;

float EPSILON = 0.0001;
uniform float volumeBound;

// calculating in world space, then mult view and projection matrix
void EmitQuad(int startIndex, int endIndex)
{
    vec3 startDirWorld = normalize(gl_in[startIndex].gl_Position.xyz - lightPos);
    vec3 endDirWorld = normalize(gl_in[endIndex].gl_Position.xyz - lightPos);
    gl_Position = projection * view * vec4(gl_in[startIndex].gl_Position.xyz + EPSILON * startDirWorld,
        1.0f);
    EmitVertex();

    gl_Position = projection * view * vec4(gl_in[endIndex].gl_Position.xyz + EPSILON * endDirWorld,
        1.0f);
    EmitVertex();

    //gl_Position = projection * view * vec4(gl_in[startIndex].gl_Position.xyz + startDirWorld * volumeBound, 1.0f);
    gl_Position = projection * view * vec4(startDirWorld , 0.0f);
    EmitVertex();

    //gl_Position = projection * view * vec4(gl_in[endIndex].gl_Position.xyz + endDirWorld * volumeBound, 1.0f);
    gl_Position = projection * view * vec4(endDirWorld, 0.0f);
    EmitVertex();
    
    EndPrimitive();

}

void EmitOriginal()
{
    gl_Position = projection * view * vec4(gl_in[0].gl_Position.xyz, 1.0f);
    EmitVertex();

    gl_Position = projection * view * vec4(gl_in[2].gl_Position.xyz, 1.0f);
    EmitVertex();

    gl_Position = projection * view * vec4(gl_in[4].gl_Position.xyz, 1.0f);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    //vec3 e1 = worldPos[2] - worldPos[0];
    //vec3 e2 = worldPos[4] - worldPos[0];
    //vec3 e3 = worldPos[1] - worldPos[0];
    //vec3 e4 = worldPos[3] - worldPos[2];
    //vec3 e5 = worldPos[4] - worldPos[2];
    //vec3 e6 = worldPos[5] - worldPos[0];
    vec3 e1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 e2 = gl_in[4].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 e3 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 e4 = gl_in[3].gl_Position.xyz - gl_in[2].gl_Position.xyz;
    vec3 e5 = gl_in[4].gl_Position.xyz - gl_in[2].gl_Position.xyz;
    vec3 e6 = gl_in[5].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    vec3 lightDir = normalize(lightPos - gl_in[0].gl_Position.xyz);

    //face normal
    vec3 normal = normalize(cross(e1, e2));
    //is this face not in the shadow
    if (dot(normal, lightDir) > 0)
    {
        normal = normalize(cross(e3, e1));

        //is e1 the silhouette
        if (dot(normal, lightDir) <= 0.0)
        {
            EmitQuad(0, 2);
        }

        normal = normalize(cross(e4, e5));
        lightDir = normalize(lightPos - gl_in[2].gl_Position.xyz);

        //is e5 the silhouette
        if (dot(normal, lightDir) <= 0.0)
        {
            EmitQuad(2, 4);
        }

        normal = normalize(cross(e2, e6));
        lightDir = normalize(lightPos - gl_in[4].gl_Position.xyz);

        //is e2 the silhouette
        if (dot(normal, lightDir) <= 0.0)
        {
            EmitQuad(4, 0);
        }

        //front capping
        lightDir = normalize(gl_in[0].gl_Position.xyz - lightPos);
        gl_Position = projection * view *
            vec4(gl_in[0].gl_Position.xyz + lightDir  * EPSILON, 1.0f);
        EmitVertex();

        lightDir = normalize(gl_in[2].gl_Position.xyz - lightPos);
        gl_Position = projection * view *
            vec4(gl_in[2].gl_Position.xyz + lightDir  * EPSILON, 1.0f);        
        EmitVertex();

        lightDir = normalize(gl_in[4].gl_Position.xyz - lightPos);
        gl_Position = projection * view *
            vec4(gl_in[4].gl_Position.xyz + lightDir * EPSILON, 1.0f);
        EmitVertex();
        EndPrimitive();

        //back capping
        lightDir = normalize(gl_in[0].gl_Position.xyz - lightPos);
        //gl_Position = projection * view * vec4(gl_in[0].gl_Position.xyz + lightDir * volumeBound, 1.0f);
        gl_Position = projection * view * vec4(lightDir, 0.0f);
        EmitVertex();
        
        lightDir = normalize(gl_in[4].gl_Position.xyz - lightPos);
        //gl_Position = projection * view * vec4(gl_in[4].gl_Position.xyz + lightDir * volumeBound , 1.0f);
        gl_Position = projection * view * vec4(lightDir  , 0.0f);
        EmitVertex();
      

        lightDir = normalize(gl_in[2].gl_Position.xyz - lightPos);
        //gl_Position = projection * view * vec4(gl_in[2].gl_Position.xyz + lightDir * volumeBound, 1.0f);
        gl_Position = projection * view * vec4(lightDir, 0.0f);
        EmitVertex();
        EndPrimitive();

    }

    //EmitOriginal();
}
