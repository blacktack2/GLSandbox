#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out Vertex {
    vec2 uv;
} OUT;

void main() {
    OUT.uv = uv;

    gl_Position = vec4(position, 1.0);
}
