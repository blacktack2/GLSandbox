#version 460 core

in vec3 position;
in vec2 uv;

out Vertex {
    vec2 uv;
} OUT;

void main() {
    OUT.uv = uv;

    gl_Position = vec4(position, 1.0);
}
