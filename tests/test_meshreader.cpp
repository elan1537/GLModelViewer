#include <iostream>
#include "MeshReader.h"

using namespace std;

int main()
{
    MeshReader testMesh("../model/livinglab.ply");

    vector<GSVertex> vertices = testMesh.getPoints();

    for (GSVertex vertex : vertices)
    {
        cout << "x: " << vertex.x << ", y: " << vertex.y << ", z: " << vertex.z << endl;
        break;
    }
}