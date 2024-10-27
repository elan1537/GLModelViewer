#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_projection;

void main() {
    // mat4 translation = mat4(1.0);  // 단위 행렬
    // translation[3] = vec4(0.0, 0.0, -0.5, 1.0);  // X, Y 방향으로 0.1씩 평행이동

    gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);  // 위치 설정
}