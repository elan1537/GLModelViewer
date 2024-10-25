// textfile.h: interface for reading and writing text files
// www.lighthouse3d.com
//
// You may use these functions freely.
// they are provided as is, and no warranties, either implicit,
// or explicit are given
//
// Modified by Won-Ki Jeong
//
//////////////////////////////////////////////////////////////////////

using namespace std;

#include <GL/glew.h>

string readFile(const char* filePath);
GLuint createShaderProgram(const char *vertexShaderPath, const char *fragmentShaderPath);