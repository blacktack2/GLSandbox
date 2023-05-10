#version 460 core

uniform vec4 lightPosition;
uniform float lightRadius;
uniform vec3 lightColour;
uniform vec3 lightDirection;
uniform float lightAngle;

uniform vec3 lightAmbient = vec3(0.0);

uniform vec3 cameraPos;

uniform vec3 modelColour = vec3(1.0);

in Vertex {
    vec2 uv;
    vec3 normal;
    vec3 worldPos;
} IN;

out vec4 fragColour;

void main() {
    vec3 incident;
    float attenuation;
    if (lightPosition.w == 0.0) {
        incident = normalize(lightDirection);
        attenuation = 1.0;
    } else {
        incident = normalize(lightPosition.xyz - IN.worldPos);
        if (lightAngle > 0.0 && degrees(acos(dot(-incident, lightDirection))) > lightAngle){
            attenuation = 0.0;
        } else {
            vec3 dist = lightPosition.xyz - IN.worldPos;
            float distSquared = dot(dist, dist);
            if (distSquared > lightRadius * lightRadius)
                attenuation = 0.0;
            else
                attenuation = 1.0 / sqrt(distSquared);
        }
    }

    vec3 viewDir = normalize(cameraPos - IN.worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    float lambert = clamp(dot(incident, IN.normal), 0.0, 1.0);
    float rFactor = clamp(dot(halfDir, IN.normal),  0.0, 1.0);
    float specFactor = pow(rFactor, 60.0);

    vec3 attenuated = lightColour * attenuation;

    vec3 diffuse = attenuated * lambert;
    vec3 specular = attenuated * specFactor * 0.33;

    vec3 result = diffuse * modelColour;
    result += specular;
    result += lightAmbient;
    fragColour = vec4(result, 1.0);
}
