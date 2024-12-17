#version 460 core

// Gaussian Parameters
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in float aOpacity;
layout (location = 3) in vec3 aScale;
layout (location = 4) in vec4 aRot;
layout (std430, binding = 5) buffer SHBuffer {
    float shData[];
};


uniform mat4 viewMat;
uniform mat4 projMat;
uniform vec4 projParam;
uniform vec4 viewport;
uniform vec3 eye;

// Geometry Shader로 전달할 varyings
// out vec2 geom_pos; // 2D Position
// out vec4 geom_cov2D; // 2D Covariance Matrix
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

    re += b[0] * r_sh[0];
    re += b[1] * r_sh[1];
    re += b[2] * r_sh[2];
    re += b[3] * r_sh[3];
    re += b[4] * r_sh[4];
    re += b[5] * r_sh[5];
    re += b[6] * r_sh[6];
    re += b[7] * r_sh[7];
    re += b[8] * r_sh[8];
    re += b[9] * r_sh[9];
    re += b[10] * r_sh[10];
    re += b[11] * r_sh[11];
    re += b[12] * r_sh[12];
    re += b[13] * r_sh[13];
    re += b[14] * r_sh[14];
    re += b[15] * r_sh[15]; 

    gr += b[0] * g_sh[0];
    gr += b[1] * g_sh[1];
    gr += b[2] * g_sh[2];
    gr += b[3] * g_sh[3];
    gr += b[4] * g_sh[4];
    gr += b[5] * g_sh[5];
    gr += b[6] * g_sh[6];
    gr += b[7] * g_sh[7];
    gr += b[8] * g_sh[8];
    gr += b[9] * g_sh[9];
    gr += b[10] * g_sh[10];
    gr += b[11] * g_sh[11];
    gr += b[12] * g_sh[12];
    gr += b[13] * g_sh[13];
    gr += b[14] * g_sh[14];
    gr += b[15] * g_sh[15];

    bl += b[0] * b_sh[0];
    bl += b[1] * b_sh[1];
    bl += b[2] * b_sh[2];
    bl += b[3] * b_sh[3];
    bl += b[4] * b_sh[4];
    bl += b[5] * b_sh[5];
    bl += b[6] * b_sh[6];
    bl += b[7] * b_sh[7];
    bl += b[8] * b_sh[8];
    bl += b[9] * b_sh[9];
    bl += b[10] * b_sh[10];
    bl += b[11] * b_sh[11];
    bl += b[12] * b_sh[12];
    bl += b[13] * b_sh[13];
    bl += b[14] * b_sh[14];
    bl += b[15] * b_sh[15];

    return vec3(0.5f, 0.5f, 0.5f) + vec3(re, gr, bl);
}


void main() {
    // SH 데이터 로드
    float r_sh[16];
    float g_sh[16];
    float b_sh[16];

    // 첫 번째 계수는 직접 할당
    r_sh[0] = shData[0];
    g_sh[0] = shData[1];
    b_sh[0] = shData[2];

    r_sh[1] = shData[3];
    r_sh[2] = shData[4];
    r_sh[3] = shData[5];
    r_sh[4] = shData[6];
    r_sh[5] = shData[7];
    r_sh[6] = shData[8];
    r_sh[7] = shData[9];
    r_sh[8] = shData[10];
    r_sh[9] = shData[11];
    r_sh[10] = shData[12];
    r_sh[11] = shData[13];
    r_sh[12] = shData[14];
    r_sh[13] = shData[15];
    r_sh[14] = shData[16];
    r_sh[15] = shData[17];

    g_sh[1] = shData[18];
    g_sh[2] = shData[19];
    g_sh[3] = shData[20];
    g_sh[4] = shData[21];
    g_sh[5] = shData[22];
    g_sh[6] = shData[23];
    g_sh[7] = shData[24];
    g_sh[8] = shData[25];
    g_sh[9] = shData[26];
    g_sh[10] = shData[27];
    g_sh[11] = shData[28];
    g_sh[12] = shData[29];
    g_sh[13] = shData[30];
    g_sh[14] = shData[31];
    g_sh[15] = shData[32];

    b_sh[1] = shData[33];
    b_sh[2] = shData[34];
    b_sh[3] = shData[35];
    b_sh[4] = shData[36];
    b_sh[5] = shData[37];
    b_sh[6] = shData[38];
    b_sh[7] = shData[39];
    b_sh[8] = shData[40];
    b_sh[9] = shData[41];
    b_sh[10] = shData[42];
    b_sh[11] = shData[43];
    b_sh[12] = shData[44];
    b_sh[13] = shData[45];
    b_sh[14] = shData[46];
    b_sh[15] = shData[47];


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
    // mat3 W = mat3(viewMat);
    // mat2 cov2D = mat2(J * W * cov3D * transpose(J * W));
    // cov2D[0][0] += 0.3f;
    // cov2D[1][1] += 0.3f;
    // geom_cov2D = vec4(cov2D[0], cov2D[1]);

    // 3D Position to 2D Position
    vec4 p4 = projMat * t;
    // geom_pos = vec2(p4.x / p4.w, p4.y / p4.w);
    // geom_pos.x = 0.5f * (width + (geom_pos.x * width) + (2.0f * x0));
    // geom_pos.y = 0.5f * (height + (geom_pos.y * height) + (2.0f * y0));

    // compute radiance from sh;
    vec3 v = normalize(aPos - eye);
    vec3 radiance = ComputeRadianceFromSH(v, r_sh, g_sh, b_sh);

    geom_color = vec4(radiance, aOpacity);
    gl_Position = p4;
}