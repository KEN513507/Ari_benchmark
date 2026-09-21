#version 330 core

in vec3 Normal;
out vec4 FragColor;

uniform vec3 uColor;

void main() {
    vec3 n = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(n, lightDir), 0.0);
    FragColor = vec4(uColor * (0.3 + 0.7 * diff), 1.0);
}
