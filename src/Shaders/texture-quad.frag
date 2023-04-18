#version 460 core

uniform sampler2D tex;

in Vertex {
    vec2 uv;
} IN;

out vec4 fragColour;

void main() {
    fragColour = texture(tex, IN.uv);
}
