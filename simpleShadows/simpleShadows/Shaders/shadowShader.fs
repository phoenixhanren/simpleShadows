#version 330 core
//out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;

uniform vec3 lightPos;
uniform vec3 viewPos;

layout (location = 0) out vec4 Ambient;
layout (location = 1) out vec4 DiffSpec;

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);

    vec3 ambient = 0.15 * color;

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * lightColor * color;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfWayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfWayDir, normal), 0.0f), 64.0f);
    vec3 specular = spec * lightColor * color;

    Ambient = vec4(ambient, 1.0f);
    DiffSpec = vec4(diffuse + specular, 1.0f);
    //vec3 result = ambient + diffuse + specular;

    //FragColor = vec4(result, 1.0f);
}
