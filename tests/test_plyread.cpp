#include <iostream>
#include <vector>

#include <happly/happly.h>

// ply
// format binary_little_endian 1.0
// element vertex 1507744
// property float x
// property float y
// property float z
// property float nx
// property float ny
// property float nz
// property float f_dc_0
// property float f_dc_1
// property float f_dc_2
// property float f_rest_0
// property float f_rest_1
// property float f_rest_2
// property float f_rest_3
// property float f_rest_4
// property float f_rest_5
// property float f_rest_6
// property float f_rest_7
// property float f_rest_8
// property float f_rest_9
// property float f_rest_10
// property float f_rest_11
// property float f_rest_12
// property float f_rest_13
// property float f_rest_14
// property float f_rest_15
// property float f_rest_16
// property float f_rest_17
// property float f_rest_18
// property float f_rest_19
// property float f_rest_20
// property float f_rest_21
// property float f_rest_22
// property float f_rest_23
// property float f_rest_24
// property float f_rest_25
// property float f_rest_26
// property float f_rest_27
// property float f_rest_28
// property float f_rest_29
// property float f_rest_30
// property float f_rest_31
// property float f_rest_32
// property float f_rest_33
// property float f_rest_34
// property float f_rest_35
// property float f_rest_36
// property float f_rest_37
// property float f_rest_38
// property float f_rest_39
// property float f_rest_40
// property float f_rest_41
// property float f_rest_42
// property float f_rest_43
// property float f_rest_44
// property float opacity
// property float scale_0
// property float scale_1
// property float scale_2
// property float rot_0
// property float rot_1
// property float rot_2
// property float rot_3
// end_header

using namespace std;

typedef struct Vertex
{
    // 위치
    float x, y, z;
    // 법선
    float nx, ny, nz;
    // DC 특성
    float f_dc[3];
    // rest 특성 (0-44)
    float f_rest[45];
    // 불투명도
    float opacity;
    // 스케일
    float scale[3];
    // 회전 (쿼터니언)
    float rot[4];
};

vector<Vertex> loadVerticesFromPLY(const string &filename)
{
    // PLY 파일 로드
    happly::PLYData plyIn(filename);

    // vertex 엘리먼트 가져오기
    auto &element = plyIn.getElement("vertex");

    // 각 속성 데이터 가져오기
    vector<float> x = element.getProperty<float>("x");
    vector<float> y = element.getProperty<float>("y");
    vector<float> z = element.getProperty<float>("z");

    vector<float> nx = element.getProperty<float>("nx");
    vector<float> ny = element.getProperty<float>("ny");
    vector<float> nz = element.getProperty<float>("nz");

    vector<float> dc0 = element.getProperty<float>("f_dc_0");
    vector<float> dc1 = element.getProperty<float>("f_dc_1");
    vector<float> dc2 = element.getProperty<float>("f_dc_2");

    // rest 특성 로드
    vector<vector<float>> f_rest;
    for (int i = 0; i < 45; i++)
    {
        f_rest.push_back(element.getProperty<float>("f_rest_" + to_string(i)));
    }

    vector<float> opacity = element.getProperty<float>("opacity");

    vector<float> scale0 = element.getProperty<float>("scale_0");
    vector<float> scale1 = element.getProperty<float>("scale_1");
    vector<float> scale2 = element.getProperty<float>("scale_2");

    vector<float> rot0 = element.getProperty<float>("rot_0");
    vector<float> rot1 = element.getProperty<float>("rot_1");
    vector<float> rot2 = element.getProperty<float>("rot_2");
    vector<float> rot3 = element.getProperty<float>("rot_3");

    // Vertex 구조체 벡터 생성
    vector<Vertex> vertices(x.size());
    for (size_t i = 0; i < vertices.size(); i++)
    {
        vertices[i].x = x[i];
        vertices[i].y = y[i];
        vertices[i].z = z[i];

        vertices[i].nx = nx[i];
        vertices[i].ny = ny[i];
        vertices[i].nz = nz[i];

        vertices[i].f_dc[0] = dc0[i];
        vertices[i].f_dc[1] = dc1[i];
        vertices[i].f_dc[2] = dc2[i];

        for (int j = 0; j < 45; j++)
        {
            vertices[i].f_rest[j] = f_rest[j][i];
        }

        vertices[i].opacity = opacity[i];

        vertices[i].scale[0] = scale0[i];
        vertices[i].scale[1] = scale1[i];
        vertices[i].scale[2] = scale2[i];

        vertices[i].rot[0] = rot0[i];
        vertices[i].rot[1] = rot1[i];
        vertices[i].rot[2] = rot2[i];
        vertices[i].rot[3] = rot3[i];
    }

    return vertices;
}

int main()
{
    auto vertices = loadVerticesFromPLY("../model/livinglab.ply");

    cout << vertices[0].x << vertices[0].y << vertices[0].z << endl;
}