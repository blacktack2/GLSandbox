#version 460 core

uniform sampler2D skyboxTex;
uniform sampler2D albedoTex;
uniform sampler2D normalTex;
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;

uniform vec3 ambienceColour;

in Vertex {
    vec2 uv;
} IN;

out vec4 fragColour;

void main() {
	vec4 albedo = texture(albedoTex, IN.uv);
	if (albedo.w <= 0.0) {
		fragColour = texture(skyboxTex, IN.uv);
		return;
	}
	vec4 normal = texture(normalTex, IN.uv);
	vec3 diffuse  = texture(diffuseTex, IN.uv).rgb;
	vec3 specular = texture(specularTex, IN.uv).rgb;
	
	vec3 result = albedo.rgb * diffuse;
	result += specular;
	result += albedo.rgb * ambienceColour;
	fragColour = vec4(result, 1.0);
}
