#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef __APPLE__  // macOS에서 GLFW 사용
    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>
    #include <GL/glew.h>
#elif defined(_WIN32)  // Windows에서 freeglut 사용
    #include <GL/glut.h>
    #include <GL/glew.h>
#endif


#include "MeshReader.h"
#include "system.h"

#include "textfile.h"
#include "Angel.h"

// Global variables for VAO, VBO, EBO, etc.
GLuint VAO, VBO, EBO;
GLuint shaderProgram;
std::vector<Vertex> vertices;
std::vector<Face> faces;
int windowHeight = 1280;
int windowWidth = 960;


#ifdef __APPLE__
void initGLFW() {
	if(!glfwInit()) {
		std::cout << "GLFW initialization failed!" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}
#elif defined(_WIN32)
void initGLUT(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowHeight, windowWidth);
	glutCreateWindow("AAA633 - Assignment 1");
}
#endif

// Function to handle rendering
void renderScene(void)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind the VAO and draw the elements
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);

#ifdef __APPLE__
    glfwSwapBuffers(glfwGetCurrentContext());
#elif defined(_WIN32)
	glutSwapBuffers();
#endif
}

int main(int argc, char **argv)
{
#ifdef __APPLE__
	initGLFW();

	GLFWwindow* window = glfwCreateWindow(windowHeight, windowWidth, "AAA633 - Assignment 1", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
#elif defined(_WIN32)
	initGLUT(argc, argv);
#endif

	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW initialization failed!" << std::endl;
		return -1;
	}

	shaderProgram = createShaderProgram("../vshader.vert", "../fshader.frag");
	glUseProgram(shaderProgram); // Shader Program을 사용해야한다고 생성 후 반드시 명시해야함

	 // Setting up VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 view = system::lookAt(glm::vec3(0.0f, -2.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = system::perspective(45.0f, (float)windowHeight / (float)windowWidth, 0.1f, 100.0f);

	GLuint transformLoc = glGetUniformLocation(shaderProgram, "u_Transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	GLuint viewLoc = glGetUniformLocation(shaderProgram, "u_View");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "u_Projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	cout << "viewLoc: " << viewLoc << endl;
	cout << "projectionLoc: " << projectionLoc << endl;
	cout << "transformLoc: " << transformLoc << endl;

    MeshReader meshReader("../mesh-data/bunny.off");
    vertices = meshReader.getVertices();
    faces = meshReader.getFaces();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(Face), &faces[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);


#ifdef __APPLE__
	while (!glfwWindowShouldClose(window))
	{
		renderScene();
		
		glfwPollEvents();
	}
	glfwTerminate();
#elif defined(_WIN32)
	glutDisplayFunc(renderScene);
	glutMainLoop();
#endif

    return 0;
}