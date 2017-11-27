// #version 330 core
// out vec4 FragColor;

// in VS_OUT {
//     vec3 FragPos;
//     vec3 Normal;
//     vec2 TexCoords;
// } fs_in;

// uniform sampler2D diffuseTexture;

// uniform vec3 lightPos;
// uniform vec3 viewPos;
// void main()
// {
//     vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
//     vec3 normal = normalize(fs_in.Normal);
//     vec3 lightColor = vec3(1.0);

//     vec3 ambient = 0.15 * color;


//     FragColor = vec4(ambient, 1.0f);
// }
#version 330 core

uniform sampler2D DiffSpecTex;
layout (location = 0) out vec4 FragColor;

void main() {
  vec4 diffSpec = texelFetch(DiffSpecTex, ivec2(gl_FragCoord), 0);

  FragColor = vec4(diffSpec.xyz, 1);
}
