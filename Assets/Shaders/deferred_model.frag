#version 460 core

uniform sampler2D albedoTex;

in Vertex {
    vec2 uv;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec3 worldPos;
} IN;

out vec4 albedoOut;
out vec4 normalOut;

void main() {
    mat3 TBN = mat3(IN.tangent,
                    IN.bitangent,
                    IN.normal);

    vec3 normal = vec3(0, 0, 1);
    normal = normalize(TBN * normal);

    albedoOut = texture(albedoTex, IN.uv);
    normalOut = vec4(normal, 1.0);
}
