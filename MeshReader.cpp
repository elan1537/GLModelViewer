#include "MeshReader.h"


MeshReader::MeshReader(const char *filePath)
{
    string meshCode = readFile(filePath);
    parsingMesh(meshCode);
    calculateVertexNormals();
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
    cout << "Vertex: " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << endl;
    cout << "Normal: " << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z << endl;
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
        vertexLine >> vertex.position.x >> vertex.position.y >> vertex.position.z;
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

void MeshReader::calculateVertexNormals() {
    for (const auto& face: this->facesList) {
        mat3 vertices = mat3(
            this->verticesList[face.v1].position,
            this->verticesList[face.v2].position,
            this->verticesList[face.v3].position
        );

        vec3 e1 = vertices[1] - vertices[0];
        vec3 e2 = vertices[2] - vertices[0];
        vec3 normal = normalize(cross(e1, e2));

        this->verticesList[face.v1].normal += normal;
        this->verticesList[face.v2].normal += normal;
        this->verticesList[face.v3].normal += normal;
    }

    for (auto& vertex: this->verticesList) {
        vertex.normal = normalize(vertex.normal);
    }
}