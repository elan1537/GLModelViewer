#include <iostream>
#include <vector>

#ifdef __APPLE__  // macOS에서 GLFW 사용
    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>
    #include <GL/glew.h>
#elif defined(_WIN32)  // Windows에서 freeglut 사용
#include <GL/glew.h>
#include <GL/freeglut.h>
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

float diffusion_scale = 0.5;
float specular_scale = 0.5;
float ambient_scale = 0.5;
float shininess = 32.0f;

vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

struct Light {
	vec3 position;
	vec3 color;
};

vector<Light> lights = {
	{ vec3(1.2f, 1.0f, 2.0f), vec3(1.0f, 0.0f, 0.0f) },
	{ vec3(1.2f, -1.0f, 2.0f), vec3(0.0f ,0.0f, 1.0f) }
};

InstanceManager *manager;

bool trackingMouse = false;
bool trackballMove = false;

int lastX, lastY;
bool viewMode = true;

vec3 mapToSphere(float x, float y) {
	float nx = (2.0 * x - windowWidth) / windowWidth;
	float ny = (windowHeight - 2.0 * y) / windowHeight;

	float length = nx * nx + ny * ny;
	return length < 1.0f ? vec3(nx, ny, sqrt(1.0f - length)) : normalize(vec3(nx, ny, 0.0f));
}


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

	if(key == GLFW_KEY_P) viewMode = true;
	if(key == GLFW_KEY_O) viewMode = false;

	if(key == GLFW_KEY_1) diffusion_scale += 0.05;
	if(key == GLFW_KEY_3) diffusion_scale -= 0.05;

	if(key == GLFW_KEY_4) specular_scale += 0.05;
	if(key == GLFW_KEY_6) specular_scale -= 0.05;

	if(key == GLFW_KEY_7) ambient_scale += 0.05;
	if(key == GLFW_KEY_9) ambient_scale -= 0.05;

	if(key == GLFW_KEY_KP_ADD) shininess *= 2.0f;
	if(key == GLFW_KEY_MINUS) shininess /= 2.0f;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		lastX = xpos;
		lastY = ypos;

		trackingMouse = true;
		// cout << lastX << " " << lastY << endl;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		trackingMouse = false;
	}
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	if (trackingMouse) {
		vec3 p1 = mapToSphere(lastX, lastY);
		vec3 p2 = mapToSphere(xpos, ypos);

		vec3 axis = cross(p1, p2);
		float angle = acos(dot(normalize(p1), normalize(p2)));

		lastX = xpos;
		lastY = ypos;
	}
}

#elif defined(_WIN32)
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0) h = 1;

	float ratio = 1.0f * (float)w / (float)h;

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

}
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) exit(0);
	if (key == 'a') cameraPos -= normalize(cross(cameraFront, cameraUp)) * moving_scale;
	if (key == 'd') cameraPos += normalize(cross(cameraFront, cameraUp)) * moving_scale;
	if (key == 'w') cameraPos += moving_scale * cameraFront;
	if (key == 's') cameraPos -= moving_scale * cameraFront;

	glutPostRedisplay();
}

void initGLUT(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowHeight, windowWidth);
	glutCreateWindow("AAA633 - Assignment 1");

	cout << "Initialize GLUT" << endl;
}

void idle()
{
	glutPostRedisplay();
}
#endif

// Function to handle rendering
void renderScene() 
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 projectionMatrix = viewMode ? system::perspective(system::toRadian(45.0f), (float)windowHeight / (float)windowWidth, 0.1f, 100.0f) : system::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	mat4 viewMatrix = system::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	manager->renderAll(viewMatrix, projectionMatrix);

	GLuint viewLoc = glGetUniformLocation(shaderProgram, "u_view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);

	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "u_projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projectionMatrix[0][0]);

	glUniform1f(glGetUniformLocation(shaderProgram, "diffuseStrength"), diffusion_scale);
	glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), specular_scale);
	glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), ambient_scale);
	glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);

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
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#elif defined(_WIN32)
	initGLUT(argc, argv);

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle); 
#endif
	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW initialization failed!" << std::endl;
		return -1;
	}

#ifdef __APPLE__
	shaderProgram = createShaderProgram("../vshader.vert", "../fshader.frag");
	MeshReader bunnyReader("../mesh-data/bunny.off");
	MeshReader dragonReader("../mesh-data/dragon-full.off");
#elif defined(_WIN32)
	shaderProgram = createShaderProgram("vshader.vert", "fshader.frag");
	MeshReader bunnyReader("bunny.off");
	MeshReader dragonReader("dragon-full.off");
#endif
	glUseProgram(shaderProgram); // Shader Program을 사용해야한다고 생성 후 반드시 명시해야함
	glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, vec3(1.0f, 1.0f, 1.0f));

	for (size_t i = 0; i < lights.size(); ++i) {
        string positionUniform = "lights[" + std::to_string(i) + "].position";
        glUniform3fv(glGetUniformLocation(shaderProgram, positionUniform.c_str()), 1, lights[i].position);

        string colorUniform = "lights[" + std::to_string(i) + "].color";
        glUniform3fv(glGetUniformLocation(shaderProgram, colorUniform.c_str()), 1, lights[i].color);
    }

	manager = new InstanceManager(shaderProgram);
	
	Object* obj_bunny = new Object(&bunnyReader, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
	Object* obj_dragon = new Object(&dragonReader, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));

	manager->addObject(obj_bunny);
	manager->addObject(obj_dragon);

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