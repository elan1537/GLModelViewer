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

float moving_scale = 0.1;

float diffusion_scale = 0.5;
float specular_scale = 0.5;
float ambient_scale = 0.5;
float shininess = 32.0f;

vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

float zoomScale = 1.0f;

struct Light {
	vec3 position;
	vec3 color;
};

vector<Light> lights = {
	{ vec3(1.2f, 1.0f, 2.0f), vec3(1.0f, 0.0f, 0.0f) },
	{ vec3(1.2f, -1.0f, 2.0f), vec3(0.0f ,0.0f, 1.0f) },
};

InstanceManager *manager;

bool trackingMouse = false;
bool trackballMove = false;

double lastX, lastY;
bool viewMode = true;

int currentButton = -1;

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
	if (action == GLFW_PRESS) {
		trackingMouse = true;
		currentButton = button;
		glfwGetCursorPos(window, &lastX, &lastY);
	} else if (action == GLFW_RELEASE) {
		trackingMouse = false;
		currentButton = -1;
	}
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	if (trackingMouse) {
		vec3 p1 = mapToSphere(lastX, lastY);
		vec3 p2 = mapToSphere(xpos, ypos);

		if (currentButton == GLFW_MOUSE_BUTTON_LEFT) {
			vec3 axis = normalize(cross(p1, p2));
			float angle = acos(dot(p1, p2)) * 50.0f;

			if (length(axis) < 0.001f || abs(angle) < 0.001f || isnan(angle)) return;

			mat4 rotationMatrix = mat4(1.0f);
    		if (abs(axis.x) > 0.001f) rotationMatrix = RotateX(angle * axis.x) * rotationMatrix;
    		if (abs(axis.y) > 0.001f) rotationMatrix = RotateY(angle * axis.y) * rotationMatrix;
    		if (abs(axis.z) > 0.001f) rotationMatrix = RotateZ(angle * axis.z) * rotationMatrix;

			vec4 cameraFront_4 = rotationMatrix * vec4(cameraFront, 1.0f);
			vec4 cameraUp_4 = rotationMatrix * vec4(cameraUp, 1.0f);

			cameraFront = normalize(vec3(cameraFront_4.x, cameraFront_4.y, cameraFront_4.z));
			cameraUp = normalize(vec3(cameraUp_4.x, cameraUp_4.y, cameraUp_4.z));
		} else if (currentButton == GLFW_MOUSE_BUTTON_MIDDLE) {
			float panSpeed = 0.01;
			vec2 delta = vec2(xpos - lastX, ypos - lastY) * panSpeed;
			vec3 right = normalize(cross(cameraFront, cameraUp));
			cameraPos += right * delta.x;
			cameraPos -= cameraUp * delta.y;
		} else if (currentButton == GLFW_MOUSE_BUTTON_RIGHT) {
			float zoomSpeed = 0.1f;
			float delta = (ypos - lastY) * zoomSpeed;
			if (viewMode) {
				cameraPos += delta * cameraFront;
			} else {
				zoomScale += delta * 0.1f;
				zoomScale = min(max(0.1f, zoomScale), 10.0f);
			}
		}
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

	if (key == 'p') viewMode = true;
	if (key == 'o') viewMode = false;

	if (key == '1') diffusion_scale += 0.05;
	if (key == '3') diffusion_scale -= 0.05;

	if (key == '4') specular_scale += 0.05;
	if (key == '6') specular_scale -= 0.05;

	if (key == '7') ambient_scale += 0.05;
	if (key == '9') ambient_scale -= 0.05;

	if (key == '+') shininess *= 2.0f;
	if (key == '-') shininess /= 2.0f;

	glutPostRedisplay();
}

void mouseMotionCallback(int x, int y) {
    if (trackingMouse) {
        vec3 p1 = mapToSphere(lastX, lastY);
        vec3 p2 = mapToSphere(x, y);

        if (currentButton == GLUT_LEFT_BUTTON) {
            vec3 axis = normalize(cross(p1, p2));
            float angle = acos(dot(p1, p2)) * 50.0f;

            if (length(axis) < 0.001f || abs(angle) < 0.001f || isnan(angle)) return;

            mat4 rotationMatrix = mat4(1.0f);
            if (abs(axis.x) > 0.001f) rotationMatrix = RotateX(angle * axis.x) * rotationMatrix;
            if (abs(axis.y) > 0.001f) rotationMatrix = RotateY(angle * axis.y) * rotationMatrix;
            if (abs(axis.z) > 0.001f) rotationMatrix = RotateZ(angle * axis.z) * rotationMatrix;

            vec4 cameraFront_4 = rotationMatrix * vec4(cameraFront, 1.0f);
            vec4 cameraUp_4 = rotationMatrix * vec4(cameraUp, 1.0f);

            cameraFront = normalize(vec3(cameraFront_4.x, cameraFront_4.y, cameraFront_4.z));
            cameraUp = normalize(vec3(cameraUp_4.x, cameraUp_4.y, cameraUp_4.z));
        } else if (currentButton == GLUT_MIDDLE_BUTTON) {
            float panSpeed = 0.01;
            vec2 delta = vec2(x - lastX, y - lastY) * panSpeed;
            vec3 right = normalize(cross(cameraFront, cameraUp));
            cameraPos += right * delta.x;
            cameraPos -= cameraUp * delta.y;
        } else if (currentButton == GLUT_RIGHT_BUTTON) {
            float zoomSpeed = 0.1f;
            float delta = (y - lastY) * zoomSpeed;
            if (viewMode) {
                cameraPos += delta * cameraFront;
            } else {
                zoomScale += delta * 0.1f;
                zoomScale = min(max(0.1f, zoomScale), 10.0f);
            }
        }
        lastX = x;
        lastY = y;
    }
}

void mouseButtonCallback(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		trackingMouse = true;
		currentButton = button;
		lastX = x;
		lastY = y;
	} else if (state == GLUT_UP) {
		trackingMouse = false;
		currentButton = -1;
	}
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

	float aspectRatio = (float)windowHeight / (float)windowWidth;

	mat4 projectionMatrix = viewMode ? system::perspective(system::toRadian(45.0f), aspectRatio, 0.1f, 100.0f) : 
									   system::ortho(-zoomScale * aspectRatio, zoomScale * aspectRatio, -zoomScale, zoomScale, -10.0f, 10.0f);
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
	glutMouseFunc(mouseButtonCallback);
	glutMotionFunc(mouseMotionCallback);
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