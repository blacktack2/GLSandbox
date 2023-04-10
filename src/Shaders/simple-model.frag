#version 460 core

uniform vec3 modelColour = vec3(1.0);

out vec4 fragColour;

void main() {
    fragColour = vec4(modelColour, 1.0);
}
