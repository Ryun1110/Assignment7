#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;

uniform vec3  uLightPos;     // 광원 위치 (world space)
uniform vec3  uEyePos;       // 카메라 위치
uniform vec3  uKa;           // ambient 반사계수
uniform vec3  uKd;           // diffuse  반사계수
uniform vec3  uKs;           // specular 반사계수
uniform float uShininess;    // shininess (α)
uniform float uIa;           // ambient 광원 세기
uniform vec3  uLightColor;   // 광원 색(흰색)
uniform float uGamma;        // 감마 보정 값(과제: 2.2)
// -----------------------------
out vec4 FragColor;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPos - vWorldPos);

    float diff = max(dot(N, L), 0.0);

    vec3  V = normalize(uEyePos - vWorldPos);
    vec3  R = reflect(-L, N);
    float spec = pow(max(dot(R, V), 0.0), uShininess);

    vec3 colorLinear = uKa * uIa +
                       uKd * diff * uLightColor +
                       uKs * spec * uLightColor;
    vec3 colorGamma  = pow(clamp(colorLinear, 0.0, 1.0), vec3(1.0 / uGamma));
    FragColor = vec4(colorGamma, 1.0);
}
