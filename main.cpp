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
#include "Object.h"
#include "InstanceManager.h"

#include "textfile.h"
#include "Angel.h"

GLuint shaderProgram;
std::vector<Vertex> vertices;
std::vector<Face> faces;
int windowHeight = 1280;
int windowWidth = 960;

float moving_scale = 0.05;

vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

GLuint lightVAO;
vec3 lightPos(1.2f, 1.0f, 2.0f);


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


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	if(key == GLFW_KEY_W) cameraPos += moving_scale * cameraFront;
	if(key == GLFW_KEY_S) cameraPos -= moving_scale * cameraFront;
	if(key == GLFW_KEY_A) cameraPos -= normalize(cross(cameraFront, cameraUp)) * moving_scale;
	if(key == GLFW_KEY_D) cameraPos += normalize(cross(cameraFront, cameraUp)) * moving_scale;
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
void renderScene(InstanceManager instanceManager) 
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 projectionMatrix = system::perspective(system::toRadian(45.0f), (float)windowHeight / (float)windowWidth, 0.1f, 100.0f);
	mat4 viewMatrix = system::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	instanceManager.renderAll(viewMatrix, projectionMatrix);

	GLuint viewLoc = glGetUniformLocation(shaderProgram, "u_view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);

	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "u_projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projectionMatrix[0][0]);

	glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, cameraPos);

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

	glfwSetKeyCallback(window, key_callback);
#elif defined(_WIN32)
	initGLUT(argc, argv);
#endif

	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW initialization failed!" << std::endl;
		return -1;
	}

	shaderProgram = createShaderProgram("../vshader.vert", "../fshader.frag");
	glUseProgram(shaderProgram); // Shader Program을 사용해야한다고 생성 후 반드시 명시해야함

	glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, vec3(1.0f, 1.0f, 1.0f));
	glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, vec3(1.0f, 0.5f, 0.31f));
	glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, lightPos);

    MeshReader bunnyReader("../mesh-data/bunny.off");
	MeshReader dragonReader("../mesh-data/dragon-full.off");

	InstanceManager manager(shaderProgram);
	
	Object* obj_bunny = new Object(&bunnyReader, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
	Object* obj_dragon = new Object(&dragonReader, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));

	manager.addObject(obj_bunny);
	manager.addObject(obj_dragon);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

#ifdef __APPLE__
	while (!glfwWindowShouldClose(window))
	{
		renderScene(manager);
		glfwPollEvents();
	}
	glfwTerminate();
#elif defined(_WIN32)
	glutDisplayFunc(renderScene);
	glutMainLoop();
#endif

    return 0;
}