#version 460 core

uniform sampler2D albedoTex;

in Vertex {
    vec2 uv;
    vec3 normal;
    vec3 worldPos;
} IN;

out vec4 albedoOut;
out vec4 normalOut;

void main() {
    albedoOut = texture(albedoTex, IN.uv);
    normalOut = vec4(IN.normal, 1.0);
}
