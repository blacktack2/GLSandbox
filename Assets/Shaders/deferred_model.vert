#version 460 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 tangent;

out Vertex {
    vec2 uv;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec3 worldPos;
} OUT;

void main() {
    OUT.uv = uv;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    vec3 worldNormal  = normalize(normalMatrix * normal);
    vec3 worldTangent = normalize(normalMatrix * tangent.xyz);

    OUT.normal    = worldNormal;
    OUT.tangent   = worldTangent;
    OUT.bitangent = cross(worldTangent, worldNormal) * tangent.w;

    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    OUT.worldPos = worldPos.xyz;

    gl_Position = projMatrix * viewMatrix * worldPos;
}
