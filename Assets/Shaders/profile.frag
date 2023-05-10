#version 460 core

uniform int bulk = 1;

out vec4 fragColour;

void main() {
    fragColour = vec4(1.0, 0.5, 0.25, 0.125);
    for (int i = 0; i < bulk; i++) {
        fragColour = fract(((fragColour * 34.0) + 1.0) * fragColour);
    }
}
