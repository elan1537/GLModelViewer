#ifndef MESHREADER_H
#define MESHREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <Angel.h>
#include <happly/happly.h>

#include <initializer_list> // 초기화 리스트를 사용하기 위한 헤더 추가

using namespace std;

struct Vertex
{
    vec3 position;
    vec3 normal = vec3(0.0f, 0.0f, 0.0f);
};

struct Face
{
    int v1, v2, v3;
};

struct GSVertex
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

class MeshReader
{
public:
    MeshReader(const char *filePath);
    MeshReader(const float* vertexArray, size_t arraySize);
    Vertex getVertex(int index);
    Face getFace(int index);
    vector<Vertex> getVertexFromFace(int faceIndex);
    vector<Vertex> getVertices();
    vector<Face> getFaces();
    vector<GSVertex> getPoints();

    void printVertices();
    void printFaces();
    void printVertex(Vertex vertex);
    void printFace(Face face);

private:
    vector<Vertex> verticesList;
    vector<Face> facesList;
    vector<GSVertex> points;
    void parsingMesh(string meshCode);
    void loadVerticesFromPLY(const string &filename);
    void calculateVertexNormals();
    string readFile(const char *filePath);
};
#endif