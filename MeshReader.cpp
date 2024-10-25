#include "MeshReader.h"
#include "textfile.h"


MeshReader::MeshReader(const char *filePath)
{
    string meshCode = readFile(filePath);
    parsingMesh(meshCode);
};

Vertex MeshReader::getVertex(int index) {
    return verticesList[index];
};

Face MeshReader::getFace(int index) {
    return facesList[index];
};

vector<Vertex> MeshReader::getVertexFromFace(int faceIndex) {
    Face face = getFace(faceIndex);

    vector<Vertex> vertices;
    for (auto faceNo: {face.v1, face.v2, face.v3}) {
        Vertex vertex = getVertex(faceNo);
        vertices.push_back(vertex);
    }

    return vertices;
}

vector<Vertex> MeshReader::getVertices() {
    return verticesList;
};

vector<Face> MeshReader::getFaces() {
    return facesList;
};

void MeshReader::printVertices() {
    for (int i = 0; i < verticesList.size(); i++) {
        printVertex(verticesList[i]);
    }
};

void MeshReader::printFaces() {
    for (int i = 0; i < facesList.size(); i++) {
        printFace(facesList[i]);
    }
};

void MeshReader::printVertex(Vertex vertex) {
    cout << "Vertex: " << vertex.x << " " << vertex.y << " " << vertex.z << endl;
};

void MeshReader::printFace(Face face) {
    cout << "Face: " << face.v1 << " " << face.v2 << " " << face.v3 << endl;
}



void MeshReader::parsingMesh(std::string meshCode) {
    istringstream meshFile(meshCode);
    string line;

    // Parsing mesh code
    // 첫번째줄은 string 'OFF'만 표시한다.
    // 두번째 줄은 vertex, face의 개수를 표시한다.

    getline(meshFile, line);
    if (line != "OFF") {
        cerr << "파일 형식이 올바르지 않습니다. OFF 파일이 아닙니다." << endl;
        return;
    }

    getline(meshFile, line);
    istringstream firstLine(line);
    int vertices, faces, vertex_per_face;
    firstLine >> vertices >> faces;

    cout << "Vertices: " << vertices << " Faces: " << faces << endl;

    for(int i=0; i<vertices; i++) {
        getline(meshFile, line);
        
        Vertex vertex;
        istringstream vertexLine(line);
        vertexLine >> vertex.x >> vertex.y >> vertex.z;
        MeshReader::verticesList.push_back(vertex);
    }

    for(int i=0; i<faces; i++) {
        getline(meshFile, line);

        Face face;
        istringstream faceLine(line);
        faceLine >> vertex_per_face >> face.v1 >> face.v2 >> face.v3;
        MeshReader::facesList.push_back(face);
    }
};