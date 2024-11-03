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
#include "Object.h"				// 오브젝트 클래스
#include "InstanceManager.h"	// 인스턴스 매니저 클래스
#include "Camera.h" 			// 카메라 클래스

#include "textfile.h"
#include "Angel.h"

// 윈도우 크기 변수
int windowHeight = 960;
int windowWidth = 1280;

// Phong 쉐이딩 관련 변수
float diffusion_scale = 0.5;
float specular_scale = 0.5;
float ambient_scale = 0.5;
float shininess = 32.0f;

struct Light {
	vec3 position;
	vec3 color;
};

vector<Light> lights = {
	{ vec3(1.2f, 1.0f, 2.0f), vec3(1.0f, 0.0f, 0.0f) },
	{ vec3(1.2f, -1.0f, 2.0f), vec3(0.0f ,0.0f, 1.0f) },
};

// Translate, Rotate, Zoom 관련 스케일 변수
float zoomScale = 0.05f;	
float rotateScale = 1.0f;
float moveScale = 0.005f;

// 카메라 관련
vec3 initialPosition = vec3(0.0f, 0.0f, 2.0f); 	// 카메라 위치 (z축 양의 방향)
vec3 targetPosition = vec3(0.0f, 0.0f, 0.0f);   // 바라보는 대상 (원점)
vec3 upVector = vec3(0.0f, 1.0f, 0.0f);         // 업 벡터

Camera camera(initialPosition, targetPosition, upVector);

// 투영 및 뷰 행렬
mat4 projectionMatrix;
mat4 viewMatrix;

// 트랙볼 관련
bool trackingMouse = false;
double lastX, lastY;

int currentButton = -1;		// 마우스 현재 버튼 상태값

// 렌더링 및 쉐이더 관련
InstanceManager *manager;
GLuint shaderProgram;


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

	if (key == GLFW_KEY_W) camera.move(camera.getFront() * moveScale);
    if (key == GLFW_KEY_S) camera.move(-camera.getFront() * moveScale);
    if (key == GLFW_KEY_A) camera.move(-camera.getRight() * moveScale);
    if (key == GLFW_KEY_D) camera.move(camera.getRight() * moveScale);
	if (key == GLFW_KEY_Q) camera.move(camera.getUp() * moveScale);
	if (key == GLFW_KEY_E) camera.move(-camera.getUp() * moveScale);

	if(key == GLFW_KEY_P) camera.isPerspective = true;
	if(key == GLFW_KEY_O) camera.isPerspective = false;

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
		float deltaX = (xpos - lastX);
		float deltaY = (ypos - lastY);

		if (currentButton == GLFW_MOUSE_BUTTON_LEFT) {
			camera.rotate(deltaX * rotateScale, deltaY * rotateScale);
		} else if (currentButton == GLFW_MOUSE_BUTTON_MIDDLE) {
			camera.pan(deltaX * moveScale, -deltaY * moveScale);
		} else if (currentButton == GLFW_MOUSE_BUTTON_RIGHT) {
			camera.zoom(deltaY * zoomScale);
		}
		lastX = xpos;
		lastY = ypos;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
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
	// 프로그램 종료
	if (key == 27) exit(0);

	// 카메라 이동 관련
	if (key == 'w') camera.move(camera.getFront() * moveScale);
    if (key == 's') camera.move(-camera.getFront() * moveScale);
    if (key == 'a') camera.move(-camera.getRight() * moveScale);
    if (key == 'd') camera.move(camera.getRight() * moveScale);
	if (key == 'q') camera.move(camera.getUp() * moveScale);
	if (key == 'e') camera.move(-camera.getUp() * moveScale);

	// P와 O키를 누르면 Perspective & Ortho 투영으로 변경
	if(key == 'p') camera.isPerspective = true;
	if(key == 'o') camera.isPerspective = false;

	// Diffusion 파라미터 변경
	if (key == '1') diffusion_scale += 0.05;
	if (key == '3') diffusion_scale -= 0.05;

	// Specular 파라미터 변경
	if (key == '4') specular_scale += 0.05;
	if (key == '6') specular_scale -= 0.05;

	// Ambient 파라미터 변경
	if (key == '7') ambient_scale += 0.05;
	if (key == '9') ambient_scale -= 0.05;

	// Shininess 변경
	if (key == '+') shininess *= 2.0f;
	if (key == '-') shininess /= 2.0f;

	glutPostRedisplay();
}

void mouseMotionCallback(int xpos, int ypos) {
	if (trackingMouse) {
		float deltaX = (xpos - lastX);
		float deltaY = (ypos - lastY);

		if (currentButton == GLUT_LEFT_BUTTON) {
			camera.rotate(deltaX * rotateScale, deltaY * rotateScale);
		} else if (currentButton == GLUT_MIDDLE_BUTTON) {
			camera.pan(deltaX * moveScale, -deltaY * moveScale);
		} else if (currentButton == GLUT_RIGHT_BUTTON) {
			camera.zoom(deltaY * zoomScale);
		}
		lastX = xpos;
		lastY = ypos;
	}
}

void mouseButtonCallback(int button, int state, int xpos, int ypos) {
	if (state == GLUT_DOWN) {
		trackingMouse = true;
		currentButton = button;
		lastX = xpos;
		lastY = ypos;
	} else if (state == GLUT_UP) {
		trackingMouse = false;
		currentButton = -1;
	}
}

void initGLUT(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("AAA633 - Assignment 1");

	cout << "Initialize GLUT" << endl;
}

void idle()
{
	glutPostRedisplay();
}
#endif

// 렌더링
void renderScene() 
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float aspectRatio = (float)windowWidth / (float)windowHeight;
	projectionMatrix = camera.isPerspective ? Perspective(45.0f, aspectRatio, 0.1f, 100.0f) : Ortho(-aspectRatio * camera.orthographicsScale, aspectRatio * camera.orthographicsScale, -camera.orthographicsScale, camera.orthographicsScale, -10, 10);

	// Phong 쉐이딩 파라미터 전달
	glUniform1f(glGetUniformLocation(shaderProgram, "diffuseStrength"), diffusion_scale);
	glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), specular_scale);
	glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), ambient_scale);
	glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);
	glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, camera.position);

	// 뷰와 투영 행렬을 객체 렌더링 전에 설정
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_view"), 1, GL_TRUE, &camera.viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_projection"), 1, GL_TRUE, &projectionMatrix[0][0]);

	// 등록된 오브젝트 렌더링
	manager->renderAll();

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

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "AAA633 - Assignment 1", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
	shaderProgram = createShaderProgram("vshader.vert", "fshader.frag"); // 쉐이더 등록
	glUseProgram(shaderProgram); // Shader Program을 사용해야한다고 생성 후 반드시 명시해야함

	MeshReader bunnyReader("bunny.off");								 // 토끼 Mesh 리더
	MeshReader dragonReader("dragon-full.off");							 // 용 Mesh 리더
	MeshReader fandiskReader("fandisk.off");						 	 // 팬디스크 Mesh 리더	

	glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, vec3(1.0f, 1.0f, 1.0f)); // 오브젝트 컬러는 흰색으로 고정 (조명들의 조합을 확인하기 쉬우라고)

	// Lighting 위치와 색 전달
	for (size_t i = 0; i < lights.size(); ++i) {
        string positionUniform = "lights[" + std::to_string(i) + "].position";
        glUniform3fv(glGetUniformLocation(shaderProgram, positionUniform.c_str()), 1, lights[i].position);

        string colorUniform = "lights[" + std::to_string(i) + "].color";
        glUniform3fv(glGetUniformLocation(shaderProgram, colorUniform.c_str()), 1, lights[i].color);
    }

	// 객체 렌더링을 위한 InstanceManager 등록
	manager = new InstanceManager(shaderProgram);
	
	Object* obj_bunny = new Object(&bunnyReader, vec3(-0.3f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
	Object* obj_dragon = new Object(&dragonReader, vec3(0.0f, 0.0f, 0.5f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
	Object* obj_fandisk = new Object(&fandiskReader, vec3(0.3f, 0.0f, 0.0f), vec3(30.0f, 0.0f, 0.0f), vec3(0.1f, 0.1f, 0.1f));

	manager->addObject(obj_bunny);
	manager->addObject(obj_dragon);
	manager->addObject(obj_fandisk);

#ifdef __APPLE__
	while (!glfwWindowShouldClose(window))
	{
		renderScene();
		glfwPollEvents();
	}
	glfwTerminate();
#elif defined(_WIN32)
	glutMainLoop();
#endif

    return 0;
}