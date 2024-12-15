#version 410 core

// Gaussian Parameters
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in float aOpacity;
layout (location = 3) in vec3 aScale;
layout (location = 4) in vec4 aRot;

uniform samplerBuffer u_shBuffer; // SH 데이터용 texture buffer
const int SH_SIZE = 48; // SH<3> 크기

uniform mat4 viewMat;
uniform mat4 projMat;
uniform vec4 projParam;
uniform vec4 viewport;
uniform vec3 eye;

// Geometry Shader로 전달할 varyings
out vec2 geom_pos; // 2D Position
out vec4 geom_cov2D; // 2D Covariance Matrix
out vec4 geom_color; // radiance (R,G,B,A)

mat3 quatToMat3(vec4 q) {
    vec4 nq = normalize(q);
    float xx = nq.x * nq.x;
    float yy = nq.y * nq.y;
    float zz = nq.z * nq.z;
    float xy = nq.x * nq.y;
    float xz = nq.x * nq.z;
    float yz = nq.y * nq.z;
    float wx = nq.w * nq.x;
    float wy = nq.w * nq.y;
    float wz = nq.w * nq.z;

    return mat3(
        1.0 - 2.0*(yy+zz), 2.0*(xy+wz),     2.0*(xz - wy),
        2.0*(xy - wz),     1.0 - 2.0*(xx+zz), 2.0*(yz+wx),
        2.0*(xz+wy),       2.0*(yz-wx),       1.0 - 2.0*(xx+yy)
    );
}

vec3 ComputeRadianceFromSH(const vec3 v, const float r_sh[16], const float g_sh[16], const float b_sh[16])
{
    float b[16];

    float vx2 = v.x * v.x;
    float vy2 = v.y * v.y;
    float vz2 = v.z * v.z;

    // zeroth order
    b[0] = 0.28209479177387814f;

    // first order
    float k1 = 0.4886025119029199f;
    b[1] = -k1 * v.y;
    b[2] = k1 * v.z;
    b[3] = -k1 * v.x;

    // second order
    float k2 = 1.0925484305920792f;
    float k3 = 0.31539156525252005f;
    float k4 = 0.5462742152960396f;

    b[4] = k2 * v.y * v.x;
    b[5] = -k2 * v.y * v.z;
    b[6] = k3 * (3.0f * vz2 - 1.0f);
    b[7] = -k2 * v.x * v.z;
    b[8] = k4 * (vx2 - vy2);

    // third order
    float k5 = 0.5900435899266435f;
    float k6 = 2.8906114426405543f;
    float k7 = 0.4570457994644658f;
    float k8 = 0.37317633259011546f;
    float k9 = 1.4453057213202771f;

    b[9] = -k5 * v.y * (3.0f * vx2 - vy2);
    b[10] = k6 * v.y * v.x * v.z;
    b[11] = -k7 * v.y * (5.0f * vz2 - 1.0f);
    b[12] = k8 * v.z * (5.0f * vz2 - 3.0f);
    b[13] = -k7 * v.x * (5.0f * vz2 - 1.0f);
    b[14] = k9 * v.z * (vx2 - vy2);
    b[15] = -k5 * v.x * (vx2 - 3.0f * vy2);

    float re = 0.0f;
    float gr = 0.0f;
    float bl = 0.0f;

    for(int i=0; i<16; i++) {
        re += b[i] * r_sh[i];
        gr += b[i] * g_sh[i];
        bl += b[i] * b_sh[i];
    }

    return vec3(0.5f, 0.5f, 0.5f) + vec3(re, gr, bl);
}


void main() {
    // SH 데이터 로드
    int pointIndex = gl_VertexID;
    int baseIndex = pointIndex * SH_SIZE;

    float r_sh[16];
    float g_sh[16];
    float b_sh[16];

    r_sh[0] = texelFetch(u_shBuffer, baseIndex + 0).r;
    g_sh[0] = texelFetch(u_shBuffer, baseIndex + 1).r;
    b_sh[0] = texelFetch(u_shBuffer, baseIndex + 2).r;

    for(int i=1; i<16; i++) {
        r_sh[i] = texelFetch(u_shBuffer, baseIndex + i + 3).r;
    }
    for(int i=1; i<16; i++) {
        g_sh[i] = texelFetch(u_shBuffer, baseIndex + i + 18).r;
    }
    for(int i=1; i<16; i++) {
        b_sh[i] = texelFetch(u_shBuffer, baseIndex + i + 33).r;
    }

    vec4 t = viewMat * vec4(aPos, 1.0);

    float x0 = viewport.x * (0.00001f * projParam.y);
    float y0 = viewport.y;

    float width = viewport.z;
    float height = viewport.w;

    float z_near = projParam.y;
    float z_far = projParam.z;

    float SX = projMat[0][0];
    float SY = projMat[1][1];
    float WZ = projMat[3][2];

    float tzSq = t.z * t.z;
    float jsx = -(SX * width) / (2.0f * t.z);
    float jsy = -(SY * height) / (2.0f * t.z);

    float jtx = (SX * t.x * width) / (2.0f * tzSq);
    float jty = (SY * t.y * height) / (2.0f * tzSq);
    float jtz = ((z_far - z_near) * WZ) / (2.0f * tzSq);
    mat3 J = mat3(vec3(jsx, 0, 0), vec3(0, jsy, 0), vec3(jtx, jty, jtz));  

    // 3D Covariance Matrix
    mat3 R = quatToMat3(aRot);
    mat3 scalemat = mat3(
        aScale.x * aScale.x, 0, 0,
        0, aScale.y * aScale.y, 0,
        0, 0, aScale.z * aScale.z
    );
    mat3 cov3D = R * scalemat * transpose(R);

    // 3D Covariance matrix to 2D Covariance matrix
    mat3 W = mat3(viewMat);
    mat2 cov2D = mat2(J * W * cov3D * transpose(J * W));
    cov2D[0][0] += 0.3f;
    cov2D[1][1] += 0.3f;
    geom_cov2D = vec4(cov2D[0], cov2D[1]);

    // 3D Position to 2D Position
    vec4 p4 = projMat * t;
    geom_pos = vec2(p4.x / p4.w, p4.y / p4.w);
    geom_pos.x = 0.5f * (width + (geom_pos.x * width) + (2.0f * x0));
    geom_pos.y = 0.5f * (height + (geom_pos.y * height) + (2.0f * y0));

    // compute radiance from sh;
    vec3 v = normalize(aPos - eye);
    vec3 radiance = ComputeRadianceFromSH(v, r_sh, g_sh, b_sh);

    geom_color = vec4(radiance, 0.01);
    gl_Position = p4;
}