#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;

}

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
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);

    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);

    FragColor = vec4(result, 1.0);
    //FragColor = vec4(vec3(shadow), 1.0);
}