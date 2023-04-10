#version 460 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

void main() {
    gl_Position = modelMatrix * viewMatrix * projMatrix * vec4(position, 1.0);
}
