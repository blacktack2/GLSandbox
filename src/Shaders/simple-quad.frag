#version 460 core

uniform vec3 quadColour = vec3(1.0);

out vec4 fragColour;

void main() {
    fragColour = vec4(quadColour, 1.0);
}
