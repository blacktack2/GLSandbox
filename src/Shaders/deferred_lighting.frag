#version 460 core

uniform sampler2D depthTex;
uniform sampler2D normalTex;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform vec4  lightPosition;
uniform float lightRadius;
uniform vec3  lightColour;
uniform vec3  lightDirection;
uniform float lightAngle;

out vec4 diffuseOut;
out vec4 specularOut;

void main() {
    vec2 uv = vec2(gl_FragCoord.xy * pixelSize);
    float depth = texture(depthTex, uv).r;
    if (depth == 1.0) // Ignore Skybox
        discard;
    vec3 ndcPos = vec3(uv, depth) * 2.0 - 1.0;
    vec4 invClipPos = inverse(projMatrix * viewMatrix) * vec4(ndcPos, 1.0);
    vec3 worldPos = invClipPos.xyz / invClipPos.w;

    vec3 incident;
    float attenuation;
    if (lightPosition.w == 0.0) { // Directional Light
        incident = normalize(lightDirection);
        attenuation = 1.0;
    } else {                      // Point/Spot Light
        incident = normalize(lightPosition.xyz - worldPos);
        if (lightAngle > 0.0 && degrees(acos(dot(-incident, lightDirection))) > lightAngle)
            discard;
        vec3 dist = lightPosition.xyz - worldPos;
        float distSquared = dot(dist, dist);
        if (distSquared > lightRadius * lightRadius)
            discard;
        attenuation = 1.0 / sqrt(distSquared);
        if (attenuation <= 0.0)
            discard;
    }

    vec3 normal = normalize(texture(normalTex, uv).xyz * 2.0 - 1.0);
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    float lambert = clamp(dot(incident, normal), 0.0, 1.0);
    float rFactor = clamp(dot(halfDir, normal),  0.0, 1.0);
    float specFactor = pow(rFactor, 60.0);

    vec3 attenuated = lightColour * attenuation;

    diffuseOut = vec4(attenuated * lambert, 1.0);
    specularOut = vec4(attenuated * specFactor * 0.33, 1.0);
}
