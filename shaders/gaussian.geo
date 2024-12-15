#version 410 core

uniform vec4 viewport;

layout(points) in;
layout(line_strip, max_vertices = 16) out;

in vec4 geom_color[];
in vec4 geom_cov2D[];
in vec2 geom_pos[];

out vec2 frag_pos;
out vec4 frag_color;   // Fragment Shader로 넘길 color

void main() {
    float width = viewport.z;
    float height = viewport.w;

    mat2 cov2D = mat2(geom_cov2D[0].xy, geom_cov2D[0].zw);

    vec4 p4 = gl_in[0].gl_Position;
    vec3 ndcP = p4.xyz / p4.w;

    if (ndcP.z < 0.25f || ndcP.x > 2.0f || ndcP.x < -2.0f || ndcP.y > 2.0f || ndcP.y < -2.0f) {
        return;
    }

    float a = cov2D[0][0];
    float b = cov2D[0][1];
    float c = cov2D[1][1];
    float apco2 = (a + c) / 2.0f;
    float amco2 = (a - c) / 2.0f;
    float term = sqrt(amco2 * amco2 + b * b);
    float maj = apco2 + term;
    float min = apco2 - term;

    float theta;
    if (b == 0.0f)
    {
        theta = (a >= c) ? 0.0f : radians(90.0f);
    }
    else
    {
        theta = atan(maj - a, b);
    }

    // k: 타원 스케일링 계수 (가우시안 "등고선"을 몇 시그마 거리로 볼지 결정)
    float k = 3.5f;
    float r1 = k * sqrt(maj);
    float r2 = k * sqrt(min);

    // 장축/단축에 따른 타원 파라미터
    vec2 majAxis = vec2(r1 * cos(theta), r1 * sin(theta));
    vec2 minAxis = vec2(r2 * cos(theta + radians(90.0f)), r2 * sin(theta + radians(90.0f)));

    vec2 center = geom_pos[0];

    const int NUM_SEGMENTS = 16;
    float delta = 2.0 * 3.141592653589793 / float(NUM_SEGMENTS);

    frag_color = geom_color[0];

    for(int i=0; i<=NUM_SEGMENTS; i++) {
        float angle = delta * float(i);
        vec2 ellipsePos = center + majAxis * cos(angle) + minAxis * sin(angle);

        float x_ndc = (ellipsePos.x * 2.0 / width) - 1.0;
        float y_ndc = (ellipsePos.y * 2.0 / height) - 1.0;


        frag_pos = vec2(x_ndc, y_ndc);
        gl_Position = vec4(x_ndc, y_ndc, 0.0, 1.0);
        EmitVertex();
    }

    EndPrimitive();
}