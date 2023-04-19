#version 460 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec2 uv;
in vec3 normal;

out Vertex {
    vec2 uv;
    vec3 normal;
    vec3 worldPos;
} OUT;

void main() {
    OUT.uv = uv;
    OUT.normal = normal;
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    OUT.worldPos = worldPos.xyz;
    gl_Position = projMatrix * viewMatrix * worldPos;
}
