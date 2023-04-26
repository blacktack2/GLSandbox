#version 460 core

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float lightRadius;
uniform vec4 lightPosition;

layout(location = 0) in vec3 position;

void main() {
    if (lightPosition.w == 0.0) { // Directional Light
        gl_Position = vec4(position, 1.0);
    } else {                      //Point/Spot light
        vec3 scale = vec3(lightRadius);
        vec3 worldPos = position * scale + lightPosition.xyz;
        gl_Position = projMatrix * viewMatrix * vec4(worldPos, 1.0);
    }
}
