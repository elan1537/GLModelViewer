#version 460 core

uniform vec4 viewport;

layout(points) in;
layout(points, max_vertices = 1) out;

in vec4 geom_color[];  // Vertex Shader에서 전달된 색상
// cov2D나 geom_pos는 필요 없으므로 제거

out vec4 frag_color; // Fragment Shader로 넘길 color

void main() {
    // 입력 점 하나에 대해 동일한 위치와 색상을 그대로 출력
    gl_Position = gl_in[0].gl_Position;
    frag_color = geom_color[0];

    EmitVertex();
    EndPrimitive();
}