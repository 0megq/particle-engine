#version 330

const vec3 lightDir = vec3(0.707,0,0.707);
const vec3 lightColor = vec3(1,1,1);
const vec4 ambientLight = vec4(0.3,0.3,0,0);

in vec3 fragNormal;
in vec4 fragColor;

out vec4 finalColor;

void main() {
    vec3 normal = normalize(fragNormal);
    float NdotL = max(dot(normal, lightDir), 0.0);
    finalColor = fragColor * (vec4((lightColor * NdotL), 1.0) + ambientLight);
}