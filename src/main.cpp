// #include <iostream>
// #include <vector>

// #ifdef __APPLE__ // macOS에서 GLFW 사용
// #define GLFW_INCLUDE_NONE
// #include <GLFW/glfw3.h>
// #include <GL/glew.h>
// #elif defined(_WIN32) // Windows에서 freeglut 사용
// #include <GL/glew.h>
// #include <GL/freeglut.h>
// #endif

// #include "MeshReader.h"
// #include "Object.h"			 // 오브젝트 클래스
// #include "InstanceManager.h" // 인스턴스 매니저 클래스
// #include "Camera.h"			 // 카메라 클래스

// #include "Shader.h" // 쉐이더 클래스
// #include "Angel.h"
// #include "Dataset.h" // Datset 클래스

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

// // 윈도우 크기 변수
// int windowHeight = 960;
// int windowWidth = 1280;

// // Phong 쉐이딩 관련 변수
// float diffusion_scale = 0.5;
// float specular_scale = 0.5;
// float ambient_scale = 0.5;
// float shininess = 32.0f;

// struct Light
// {
// 	vec3 position;
// 	vec3 color;
// };

// vector<Light> lights = {
// 	{vec3(1.2f, 1.0f, 2.0f), vec3(1.0f, 0.0f, 0.0f)},
// 	{vec3(1.2f, -1.0f, 2.0f), vec3(0.0f, 0.0f, 1.0f)},
// };

// // Translate, Rotate, Zoom 관련 스케일 변수
// float zoomScale = 0.05f;
// float rotateScale = 1.0f;
// float moveScale = 0.005f;

// // 카메라 관련
// vec3 initialPosition = vec3(0.0f, 0.0f, 2.0f); // 카메라 위치 (z축 양의 방향)
// vec3 targetPosition = vec3(0.0f, 0.0f, 0.0f);  // 바라보는 대상 (원점)
// vec3 upVector = vec3(0.0f, 1.0f, 0.0f);		   // 업 벡터

// Camera camera(initialPosition, targetPosition, upVector);

// Shader *meshShader;
// Shader *gsShader;

// // 투영 및 뷰 행렬
// mat4 projectionMatrix;
// mat4 viewMatrix;

// // 트랙볼 관련
// bool trackingMouse = false;
// double lastX, lastY;

// int currentButton = -1; // 마우스 현재 버튼 상태값

// // 렌더링 및 쉐이더 관련
// InstanceManager *manager;
// GLuint shaderProgram;

// // 전역 변수로 마우스 상태 추가
// struct MouseState
// {
// 	bool leftPressed = false;
// 	bool rightPressed = false;
// 	bool middlePressed = false;
// 	double xpos = 0.0;
// 	double ypos = 0.0;
// } mouseState;

// #ifdef __APPLE__
// void initGLFW()
// {
// 	if (!glfwInit())
// 	{
// 		std::cout << "GLFW initialization failed!" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
// 	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// }

// void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
// {
// 	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
// 	{
// 		glfwSetWindowShouldClose(window, GLFW_TRUE);
// 	}

// 	if (key == GLFW_KEY_W)
// 		camera.move(camera.getFront() * moveScale);
// 	if (key == GLFW_KEY_S)
// 		camera.move(-camera.getFront() * moveScale);
// 	if (key == GLFW_KEY_A)
// 		camera.move(-camera.getRight() * moveScale);
// 	if (key == GLFW_KEY_D)
// 		camera.move(camera.getRight() * moveScale);
// 	if (key == GLFW_KEY_Q)
// 		camera.move(camera.getUp() * moveScale);
// 	if (key == GLFW_KEY_E)
// 		camera.move(-camera.getUp() * moveScale);

// 	if (key == GLFW_KEY_P)
// 		camera.isPerspective = true;
// 	if (key == GLFW_KEY_O)
// 		camera.isPerspective = false;

// 	if (key == GLFW_KEY_1)
// 		diffusion_scale += 0.05;
// 	if (key == GLFW_KEY_3)
// 		diffusion_scale -= 0.05;

// 	if (key == GLFW_KEY_4)
// 		specular_scale += 0.05;
// 	if (key == GLFW_KEY_6)
// 		specular_scale -= 0.05;

// 	if (key == GLFW_KEY_7)
// 		ambient_scale += 0.05;
// 	if (key == GLFW_KEY_9)
// 		ambient_scale -= 0.05;

// 	if (key == GLFW_KEY_KP_ADD)
// 		shininess *= 2.0f;
// 	if (key == GLFW_KEY_MINUS)
// 		shininess /= 2.0f;
// }

// void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
// {
// 	// ImGui가 마우스를 사용중인지 먼저 확인
// 	ImGuiIO &io = ImGui::GetIO();
// 	if (io.WantCaptureMouse)
// 	{
// 		return; // ImGui가 마우스를 사용중이면 여기서 종료
// 	}

// 	// ImGui가 마우스를 사용하지 않을 때만 카메라 컨트롤 처리
// 	if (action == GLFW_PRESS)
// 	{
// 		trackingMouse = true;
// 		currentButton = button;
// 		glfwGetCursorPos(window, &lastX, &lastY);
// 	}
// 	else if (action == GLFW_RELEASE)
// 	{
// 		trackingMouse = false;
// 		currentButton = -1;
// 	}
// }

// void cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
// {
// 	mouseState.xpos = xpos;
// 	mouseState.ypos = ypos;
// }

// void framebuffer_size_callback(GLFWwindow *window, int width, int height)
// {
// 	windowWidth = width;
// 	windowHeight = height;
// 	glViewport(0, 0, width, height);
// }

// #elif defined(_WIN32)
// void changeSize(int w, int h)
// {

// 	// Prevent a divide by zero, when window is too short
// 	// (you cant make a window of zero width).
// 	if (h == 0)
// 		h = 1;

// 	float ratio = 1.0f * (float)w / (float)h;

// 	// Set the viewport to be the entire window
// 	glViewport(0, 0, w, h);
// }
// void keyboard(unsigned char key, int x, int y)
// {
// 	// 프로그램 종료
// 	if (key == 27)
// 		exit(0);

// 	// 카메라 이동 관련
// 	if (key == 'w')
// 		camera.move(camera.getFront() * moveScale);
// 	if (key == 's')
// 		camera.move(-camera.getFront() * moveScale);
// 	if (key == 'a')
// 		camera.move(-camera.getRight() * moveScale);
// 	if (key == 'd')
// 		camera.move(camera.getRight() * moveScale);
// 	if (key == 'q')
// 		camera.move(camera.getUp() * moveScale);
// 	if (key == 'e')
// 		camera.move(-camera.getUp() * moveScale);

// 	// P와 O키를 누르면 Perspective & Ortho 투영으로 변경
// 	if (key == 'p')
// 		camera.isPerspective = true;
// 	if (key == 'o')
// 		camera.isPerspective = false;

// 	// Diffusion 파라미터 변경
// 	if (key == '1')
// 		diffusion_scale += 0.05;
// 	if (key == '3')
// 		diffusion_scale -= 0.05;

// 	// Specular 파라미터 변경
// 	if (key == '4')
// 		specular_scale += 0.05;
// 	if (key == '6')
// 		specular_scale -= 0.05;

// 	// Ambient 파라미터 변경
// 	if (key == '7')
// 		ambient_scale += 0.05;
// 	if (key == '9')
// 		ambient_scale -= 0.05;

// 	// Shininess 변경
// 	if (key == '+')
// 		shininess *= 2.0f;
// 	if (key == '-')
// 		shininess /= 2.0f;

// 	glutPostRedisplay();
// }

// void mouseMotionCallback(int xpos, int ypos)
// {
// 	if (trackingMouse)
// 	{
// 		float deltaX = (xpos - lastX);
// 		float deltaY = (ypos - lastY);

// 		if (currentButton == GLUT_LEFT_BUTTON)
// 		{
// 			camera.rotate(deltaX * rotateScale, deltaY * rotateScale);
// 		}
// 		else if (currentButton == GLUT_MIDDLE_BUTTON)
// 		{
// 			camera.pan(deltaX * moveScale, -deltaY * moveScale);
// 		}
// 		else if (currentButton == GLUT_RIGHT_BUTTON)
// 		{
// 			camera.zoom(deltaY * zoomScale);
// 		}
// 		lastX = xpos;
// 		lastY = ypos;
// 	}
// }

// void mouseButtonCallback(int button, int state, int xpos, int ypos)
// {
// 	if (state == GLUT_DOWN)
// 	{
// 		trackingMouse = true;
// 		currentButton = button;
// 		lastX = xpos;
// 		lastY = ypos;
// 	}
// 	else if (state == GLUT_UP)
// 	{
// 		trackingMouse = false;
// 		currentButton = -1;
// 	}
// }

// void initGLUT(int argc, char **argv)
// {
// 	glutInit(&argc, argv);
// 	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
// 	glutInitWindowSize(windowWidth, windowHeight);
// 	glutCreateWindow("AAA633 - Assignment 1");

// 	cout << "Initialize GLUT" << endl;
// }

// void idle()
// {
// 	glutPostRedisplay();
// }
// #endif

// // 렌더링
// void renderScene()
// {
// 	glEnable(GL_DEPTH_TEST);
// 	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 	float aspectRatio = (float)windowWidth / (float)windowHeight;
// 	projectionMatrix = camera.isPerspective ? Perspective(45.0f, aspectRatio, 0.1f, 100.0f) : Ortho(-aspectRatio * camera.orthographicsScale, aspectRatio * camera.orthographicsScale, -camera.orthographicsScale, camera.orthographicsScale, -10, 10);

// 	// // Phong 쉐이딩 파라미터 전달
// 	// shader->setFloat("diffuseStrength", diffusion_scale);
// 	// shader->setFloat("specularStrength", specular_scale);
// 	// shader->setFloat("ambientStrength", ambient_scale);
// 	// shader->setFloat("shininess", shininess);
// 	// shader->setVec3("viewPos", camera.position);

// 	// 뷰와 투영 행렬을 객체 렌더링 전에 설정
// 	meshShader->setMat4("u_view", camera.viewMatrix, GL_TRUE);
// 	meshShader->setMat4("u_projection", projectionMatrix, GL_TRUE);

// 	gsShader->setMat4("u_view", camera.viewMatrix, GL_TRUE);
// 	gsShader->setMat4("u_projection", projectionMatrix, GL_TRUE);

// 	// 등록된 오브젝트 렌더링
// 	manager->renderAll();

// #ifdef defined(_WIN32)
// 	glutSwapBuffers();
// #endif
// }

// int main(int argc, char **argv)
// {
// #ifdef __APPLE__
// 	initGLFW();

// 	GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "AAA633 - Assignment 2", NULL, NULL);
// 	if (!window)
// 	{
// 		glfwTerminate();
// 		exit(EXIT_FAILURE);
// 	}
// 	glfwMakeContextCurrent(window);

// 	if (glewInit() != GLEW_OK)
// 	{
// 		std::cerr << "GLEW initialization failed!" << std::endl;
// 		return -1;
// 	}

// 	// ImGui 초기화를 GLFW 콜백 설정 전에 수행
// 	IMGUI_CHECKVERSION();
// 	ImGui::CreateContext();
// 	ImGuiIO &io = ImGui::GetIO();
// 	(void)io;

// 	// ImGui 입력/백엔드 설정
// 	ImGui_ImplGlfw_InitForOpenGL(window, true); // true는 콜백 설치를 의미
// 	ImGui_ImplOpenGL3_Init("#version 330");
// 	ImGui::StyleColorsDark();

// 	// GLFW 콜백은 ImGui 초기화 후에 설정
// 	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

// 	// meshShader = new Shader("shader/mesh_vshader.vert", "shader/fshader.frag");
// 	gsShader = new Shader("shader/gs_vshader.vert", "shader/fshader.frag");

// 	// meshShader->use();
// 	gsShader->use();

// 	float cube[] = {
// 		// Back
// 		-1.0f, -1.0f, -1.0f,
// 		1.0f, -1.0f, -1.0f,
// 		1.0f, 1.0f, -1.0f,
// 		1.0f, 1.0f, -1.0f,
// 		-1.0f, 1.0f, -1.0f,
// 		-1.0f, -1.0f, -1.0f,

// 		// Front
// 		-1.0f, -1.0f, 1.0f,
// 		1.0f, -1.0f, 1.0f,
// 		1.0f, 1.0f, 1.0f,
// 		1.0f, 1.0f, 1.0f,
// 		-1.0f, 1.0f, 1.0f,
// 		-1.0f, -1.0f, 1.0f,

// 		// Bottom
// 		-1.0f, -1.0f, -1.0f,
// 		1.0f, -1.0f, -1.0f,
// 		1.0f, -1.0f, 1.0f,
// 		1.0f, -1.0f, 1.0f,
// 		-1.0f, -1.0f, 1.0f,
// 		-1.0f, -1.0f, -1.0f,

// 		// Top
// 		-1.0f, 1.0f, -1.0f,
// 		1.0f, 1.0f, -1.0f,
// 		1.0f, 1.0f, 1.0f,
// 		1.0f, 1.0f, 1.0f,
// 		-1.0f, 1.0f, 1.0f,
// 		-1.0f, 1.0f, -1.0f,

// 		// Left
// 		-1.0f, -1.0f, -1.0f,
// 		-1.0f, 1.0f, -1.0f,
// 		-1.0f, 1.0f, 1.0f,
// 		-1.0f, 1.0f, 1.0f,
// 		-1.0f, -1.0f, 1.0f,
// 		-1.0f, -1.0f, -1.0f,

// 		// Right
// 		1.0f, -1.0f, -1.0f,
// 		1.0f, 1.0f, -1.0f,
// 		1.0f, 1.0f, 1.0f,
// 		1.0f, 1.0f, 1.0f,
// 		1.0f, -1.0f, 1.0f,
// 		1.0f, -1.0f, -1.0f,
// 	};

// 	MeshReader *livingLab = new MeshReader("model/livinglab.ply");
// 	MeshReader *cubeObj = new MeshReader(cube, sizeof(cube) / sizeof(cube[0]));
// 	manager = new InstanceManager(*meshShader, *gsShader);
	
// 	Object *livingSpace = new Object(livingLab, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.1f, 0.1f, 0.1f), Object::RenderType::GAUSSIAN_SPLAT);
// 	Object *cubeObject = new Object(cubeObj, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.1f, 0.1f, 0.1f), Object::RenderType::MESH);
	
// 	cubeObject->addInstance(vec3(1.0f, 0.0f, 0.0f), vec3(0.0f), vec3(0.1f));
// 	cubeObject->addInstance(vec3(-1.0f, 0.0f, 0.0f), vec3(0.0f), vec3(0.1f));

// 	manager->addObject(livingSpace);
// 	manager->addObject(cubeObject);

// 	Dataset dataset("../model/livinglab-7");
// 	std::vector<CameraInfo> cameras = dataset.getCameras();
// 	int currentImageIndex = 0;

// 	// 마우스 상태 변수
// 	bool firstMouse = true;
// 	double lastX = windowWidth / 2.0f;
// 	double lastY = windowHeight / 2.0f;

// 	while (!glfwWindowShouldClose(window))
// 	{
// 		ImGui_ImplOpenGL3_NewFrame();
// 		ImGui_ImplGlfw_NewFrame();
// 		ImGui::NewFrame();

// 		// ImGui 윈도우
// 		ImGui::Begin("Controls");
// 		ImGui::Text("Camera Controls");
// 		ImGui::SliderFloat("Move Scale", &moveScale, 0.001f, 0.01f);
// 		ImGui::SliderFloat("Rotate Scale", &rotateScale, 0.1f, 2.0f);
// 		ImGui::SliderFloat("Zoom Scale", &zoomScale, 0.01f, 0.1f);

// 		// 카메라 리셋 버튼 추가
// 		if (ImGui::Button("Reset Camera"))
// 		{
// 			camera = Camera(initialPosition, targetPosition, upVector);
// 		}
// 		ImGui::End();

// 		ImGui::Begin("New Window");
// 		ImGui::Text("Train Path");

// 		if (ImGui::Button("Next Image")) {
// 			currentImageIndex = (currentImageIndex + 1) % cameras.size();
// 			CameraInfo &camera = cameras[currentImageIndex];

// 			std::cout << camera.image_name << ", " << camera.T << std::endl;
// 		}
		
// 		if (ImGui::Button("Previous Image")) {
// 			currentImageIndex = (currentImageIndex - 1 + cameras.size()) % cameras.size();
// 			CameraInfo &camera = cameras[currentImageIndex];

// 			std::cout << camera.image_name << ", " << camera.T << std::endl;
// 		}
// 		ImGui::End();

// 		// 키보드 입력 처리
// 		if (!ImGui::GetIO().WantCaptureKeyboard)
// 		{
// 			// WASD로 카메라 이동
// 			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
// 				camera.move(camera.getFront() * moveScale);
// 			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
// 				camera.move(-camera.getFront() * moveScale);
// 			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
// 				camera.move(-camera.getRight() * moveScale);
// 			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
// 				camera.move(camera.getRight() * moveScale);

// 			// QE로 카메라 상하 이동
// 			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
// 				camera.move(camera.getUp() * moveScale);
// 			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
// 				camera.move(-camera.getUp() * moveScale);

// 			// R키로 카메라 리셋
// 			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
// 				camera = Camera(initialPosition, targetPosition, upVector);

// 			// ESC로 프로그램 종료
// 			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
// 				glfwSetWindowShouldClose(window, true);
// 		}

// 		// 마우스 입력 처리
// 		if (!ImGui::GetIO().WantCaptureMouse)
// 		{
// 			double xpos, ypos;
// 			glfwGetCursorPos(window, &xpos, &ypos);

// 			if (firstMouse)
// 			{
// 				lastX = xpos;
// 				lastY = ypos;
// 				firstMouse = false;
// 			}

// 			float xoffset = xpos - lastX;
// 			float yoffset = lastY - ypos;

// 			lastX = xpos;
// 			lastY = ypos;

// 			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
// 			{
// 				camera.rotate(xoffset * rotateScale, yoffset * rotateScale);
// 			}
// 			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
// 			{
// 				camera.pan(xoffset * moveScale, -yoffset * moveScale);
// 			}
// 			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
// 			{
// 				camera.zoom(yoffset * zoomScale);
// 			}
// 		}

// 		// Scene 렌더링
// 		renderScene();

// 		ImGui::Render();
// 		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

// 		glfwSwapBuffers(window);
// 		glfwPollEvents();
// 	}

// 	// ImGui 정리
// 	ImGui_ImplOpenGL3_Shutdown();
// 	ImGui_ImplGlfw_Shutdown();
// 	ImGui::DestroyContext();

// 	glfwTerminate();
// #endif
// 	return 0;
// }