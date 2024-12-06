#include <MeshReader.h>

MeshReader::MeshReader(const char *filePath)
{
    std::filesystem::path p(filePath);

    string fileName = p.filename().string();
    string fileExt = p.extension().string();

    if (fileExt.compare(".ply") == 0)
    {
        loadVerticesFromPLY(p.string());
        cout << "success parsing the ply file" << endl;
    }
    else if (fileExt.compare(".off") == 0)
    {
        string meshCode = readFile(filePath);
        parsingMesh(meshCode);
        calculateVertexNormals();
    }
    else
    {
        cout << "Not Supported" << endl;
    }
};

vector<GSVertex> MeshReader::getPoints()
{
    return this->points;
}

void MeshReader::loadVerticesFromPLY(const string &filename)
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
    for (int i = 0; i < x.size(); i++)
    {
        GSVertex vertex;
        vertex.x = x[i];
        vertex.y = y[i];
        vertex.z = z[i];

        vertex.nx = nx[i];
        vertex.ny = ny[i];
        vertex.nz = nz[i];

        vertex.f_dc[0] = dc0[i];
        vertex.f_dc[1] = dc1[i];
        vertex.f_dc[2] = dc2[i];

        for (int j = 0; j < 45; j++)
        {
            vertex.f_rest[j] = f_rest[j][i];
        }

        vertex.opacity = opacity[i];

        vertex.scale[0] = scale0[i];
        vertex.scale[1] = scale1[i];
        vertex.scale[2] = scale2[i];

        vertex.rot[0] = rot0[i];
        vertex.rot[1] = rot1[i];
        vertex.rot[2] = rot2[i];
        vertex.rot[3] = rot3[i];

        this->points.push_back(vertex);

        float progress = (float)i / x.size() * 100.0;
        cout << "\rParsed: " << fixed << setprecision(1) << progress << "%" << flush;
    }
    cout << endl;

    for (int i = 0; i < 10; i++)
    {
        GSVertex vertex = this->points[i];

        cout << vertex.x << ", " << vertex.y << ", " << vertex.z << ", " << vertex.f_rest[0] << ", " << vertex.f_rest[1] << ", " << vertex.f_rest[2] << endl;
    }
}

Vertex MeshReader::getVertex(int index)
{
    return verticesList[index];
};

Face MeshReader::getFace(int index)
{
    return facesList[index];
};

vector<Vertex> MeshReader::getVertexFromFace(int faceIndex)
{
    Face face = getFace(faceIndex);

    vector<Vertex> vertices;
    for (auto faceNo : {face.v1, face.v2, face.v3})
    {
        Vertex vertex = getVertex(faceNo);
        vertices.push_back(vertex);
    }

    return vertices;
}

vector<Vertex> MeshReader::getVertices()
{
    return verticesList;
};

vector<Face> MeshReader::getFaces()
{
    return facesList;
};

void MeshReader::printVertices()
{
    for (int i = 0; i < verticesList.size(); i++)
    {
        printVertex(verticesList[i]);
    }
};

void MeshReader::printFaces()
{
    for (int i = 0; i < facesList.size(); i++)
    {
        printFace(facesList[i]);
    }
};

void MeshReader::printVertex(Vertex vertex)
{
    cout << "Vertex: " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << endl;
    cout << "Normal: " << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z << endl;
};

void MeshReader::printFace(Face face)
{
    cout << "Face: " << face.v1 << " " << face.v2 << " " << face.v3 << endl;
}

void MeshReader::parsingMesh(std::string meshCode)
{
    istringstream meshFile(meshCode);
    string line;

    // Parsing mesh code
    // 첫번째줄은 string 'OFF'만 표시한다.
    // 두번째 줄은 vertex, face의 개수를 표시한다.

    getline(meshFile, line);
    if (line != "OFF")
    {
        cerr << "파일 형식이 올바르지 않습니다. OFF 파일이 아닙니다." << endl;
        return;
    }

    getline(meshFile, line);
    istringstream firstLine(line);
    int vertices, faces, vertex_per_face;
    firstLine >> vertices >> faces;

    cout << "Vertices: " << vertices << " Faces: " << faces << endl;

    for (int i = 0; i < vertices; i++)
    {
        getline(meshFile, line);

        Vertex vertex;
        istringstream vertexLine(line);
        vertexLine >> vertex.position.x >> vertex.position.y >> vertex.position.z;
        MeshReader::verticesList.push_back(vertex);
    }

    for (int i = 0; i < faces; i++)
    {
        getline(meshFile, line);

        Face face;
        istringstream faceLine(line);
        faceLine >> vertex_per_face >> face.v1 >> face.v2 >> face.v3;
        MeshReader::facesList.push_back(face);
    }
};

void MeshReader::calculateVertexNormals()
{
    for (const auto &face : this->facesList)
    {
        mat3 vertices = mat3(
            this->verticesList[face.v1].position,
            this->verticesList[face.v2].position,
            this->verticesList[face.v3].position);

        vec3 e1 = vertices[1] - vertices[0];
        vec3 e2 = vertices[2] - vertices[0];
        vec3 normal = normalize(cross(e1, e2));

        this->verticesList[face.v1].normal += normal;
        this->verticesList[face.v2].normal += normal;
        this->verticesList[face.v3].normal += normal;
    }

    for (auto &vertex : this->verticesList)
    {
        vertex.normal = normalize(vertex.normal);
    }
}

string MeshReader::readFile(const char *filePath)
{
    ifstream file(filePath);
    if (!file.is_open())
    {
        cerr << "Failed to open file: " << filePath << endl;
        exit(EXIT_FAILURE);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}