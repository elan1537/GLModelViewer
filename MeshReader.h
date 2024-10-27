#ifndef MESHREADER_H
#define MESHREADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "textfile.h"
#include "Angel.h"

#include <initializer_list>  // 초기화 리스트를 사용하기 위한 헤더 추가

using namespace std;

struct Vertex {
    vec3 position;
    vec3 normal = vec3(0.0f, 0.0f, 0.0f);
};

struct Face {
    int v1, v2, v3;
};


class MeshReader {
    public:
        MeshReader(const char *filePath);
        Vertex getVertex(int index);
        Face getFace(int index);
        vector<Vertex> getVertexFromFace(int faceIndex);
        vector<Vertex> getVertices();
        vector<Face> getFaces();
        void printVertices();
        void printFaces();
        void printVertex(Vertex vertex);
        void printFace(Face face);
    private:
        vector<Vertex> verticesList;
        vector<Face> facesList;
        void parsingMesh(string meshCode);
        void calculateVertexNormals();
};
#endif 