#version 330 core

in vec3 Normal;
in float vPartID;
out vec4 FragColor;

uniform vec3 uColor;

void main() {
    vec3 n = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(n, lightDir), 0.0);
    // 複眼 (partID=5) は暗色で塗り分け。それ以外は uColor。
    vec3 base = uColor;
    if (vPartID > 4.5 && vPartID < 5.5) {
        base = vec3(0.10, 0.06, 0.04);
    }
    FragColor = vec4(base * (0.3 + 0.7 * diff), 1.0);
}
