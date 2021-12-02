#include <cstdlib>
#include "System.h"
#include <windows.h>
#include "TrackWriter.h"

void mouseCallback(GLFWwindow* window, int button, int action, int mods);
void handleKeys(GLFWwindow* window);
void handleArrowKeys(GLFWwindow* window);
void handleRestartKey(GLFWwindow* window);
void handleGenerateTrackKey(GLFWwindow* window);
void handleSaveKey(GLFWwindow* window);
void handleEscKey(GLFWwindow* window);
void bsplineCenter();
float calculateBspline(float& t, float& t_pow_3, float& t_pow_2, float& p1, float& p2, float& p3, float& p4);
void bsplineInternalExternal();
void generateTrackObj();
void validatePointClick(double posX, double posY);
void restart();
void writeInstructions();
void generateTrackDrawing(GLint color);

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

double posX, posY;

GLuint VAO_POINTS, VBO_POINTS;
GLuint VAO_CENTER, VBO_CENTER;
GLuint VAO_INTERNAL, VBO_INTERNAL;
GLuint VAO_EXTERNAL, VBO_EXTERNAL;

vector<float> points;
vector<float> center_array;
vector<float> internal_array;
vector<float> external_array;

bool drawTrack, isPointSelected = false;
int lastPoint = 0.0;

TrackWriter trackWriter;

System::System() {}

System::~System() {}

GLFWwindow* System::GLFWInit()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Pista", nullptr, nullptr);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();

		return 0;
	}

	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, mouseCallback);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed no init GLEW." << std::endl;
		return 0;
	}

	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);

	coreShader = Shader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
	coreShader.Use();
	coreShader.setMatrix4fv("projection", glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f));

	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl << endl;

	writeInstructions();

	return window;
}

void writeInstructions()
{
	cout << "ESC - Sair" << endl;
	cout << "Enter - Desenhar pista" << endl;
	cout << "S - Gerar objeto" << endl;
	cout << "R - Reiniciar" << endl;
	cout << "UP - Valor +1" << endl;
	cout << "DOWN - Valor -1" << endl;
}

void System::Run(Shader shader, GLFWwindow* window)
{
	coreShader.Use();

	glGenBuffers(1, &VBO_POINTS);
	glGenVertexArrays(1, &VAO_POINTS);

	glGenBuffers(1, &VBO_CENTER);
	glGenVertexArrays(1, &VAO_CENTER);

	glGenBuffers(1, &VBO_INTERNAL);
	glGenVertexArrays(1, &VAO_INTERNAL);

	glGenBuffers(1, &VBO_EXTERNAL);
	glGenVertexArrays(1, &VAO_EXTERNAL);

	GLint color = glGetUniformLocation(coreShader.program, "FragPos");

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		handleKeys(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		coreShader.Use();

		glUniform3f(color, 0.0, 0.0, 0.0);
		glBindVertexArray(VAO_POINTS);
		glPointSize(10);
		glDrawArrays(GL_POINTS, 0, (GLsizei)points.size() / 3);

		if (drawTrack) {
			generateTrackDrawing(color);
		}

		glfwSwapBuffers(window);
	}
}

void generateTrackDrawing(GLint color) {
	bsplineCenter();

	glUniform3f(color, 1.0, 1.0, 1.0);
	glBindVertexArray(VAO_CENTER);
	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)center_array.size() / 3);

	glUniform3f(color, 0.0, 1.0, 1.0);
	glBindVertexArray(VAO_INTERNAL);
	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)internal_array.size() / 3);

	glBindVertexArray(VAO_EXTERNAL);
	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)external_array.size() / 3);
}

void System::Finish()
{
	glDeleteVertexArrays(1, &VAO_POINTS);
	glDeleteVertexArrays(1, &VAO_CENTER);
	glDeleteVertexArrays(1, &VAO_INTERNAL);
	glDeleteBuffers(1, &VBO_POINTS);
	glDeleteBuffers(1, &VBO_CENTER);
	glDeleteBuffers(1, &VBO_INTERNAL);
	coreShader.Delete();
	glfwTerminate();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void handleKeys(GLFWwindow* window)
{
	handleEscKey(window);
	handleSaveKey(window);
	handleGenerateTrackKey(window);
	handleRestartKey(window);
	handleArrowKeys(window);
}

void handleArrowKeys(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (drawTrack && isPointSelected) {
			points[lastPoint] += 1;
			cout << "                       Valor = " << points[lastPoint] << endl;
		}
		Sleep(100);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (drawTrack && isPointSelected) {
			points[lastPoint] -= 1;
			cout << "                       Valor = " << points[lastPoint] << endl;
		}
		Sleep(100);
	}
}

void handleRestartKey(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		restart();
		Sleep(100);
	}
}

void handleGenerateTrackKey(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !drawTrack) {
		drawTrack = true;
		cout << endl << "Selecione um ponto para alterar a altura." << endl;
	}
}

void handleSaveKey(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (drawTrack) {
			generateTrackObj();
			cout << "Gerando objeto da pista...";
		}
		else {
			cout << "A pista ainda nao foi concluida!";
		}
		Sleep(300);
	}
}

void handleEscKey(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// -------------------------------------------------------
void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &posX, &posY);
		if (!drawTrack) {

			points.push_back(posX);
			points.push_back(posY);
			points.push_back(0.0f);

			glBindVertexArray(VAO_POINTS);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_POINTS);
			glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(0);
		}
		else {
			validatePointClick(posX, posY);
		}
	}
}

void bsplineCenter() {
	center_array.clear();
	internal_array.clear();
	external_array.clear();
	int size = points.size();

	for (int i = 0; i < size + 3; i += 3) {
		for (float t = 0; t < 1; t += 0.03f) {

			float t3 = pow(t, 3);
			float t2 = pow(t, 2);

			float x = calculateBspline(t, t3, t2, points[(i) % size], points[(i + 3) % size], points[(i + 6) % size], points[(i + 9) % size]);
			float y = calculateBspline(t, t3, t2, points[(i + 1) % size], points[(i + 4) % size], points[(i + 7) % size], points[(i + 10) % size]);
			float z = calculateBspline(t, t3, t2, points[(i + 2) % size], points[(i + 5) % size], points[(i + 8) % size], points[(i + 11) % size]);

			center_array.push_back(x);
			center_array.push_back(y);
			center_array.push_back(z);
		}
	}

	glBindVertexArray(VAO_CENTER);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_CENTER);
	glBufferData(GL_ARRAY_BUFFER, center_array.size() * sizeof(float), center_array.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	bsplineInternalExternal();
}

float calculateBspline(float& t, float& t_pow_3, float& t_pow_2, float& p1, float& p2, float& p3, float& p4) {
	return (
		(
			(-1 * t_pow_3 + 3 * t_pow_2 - 3 * t + 1) * p1 +
			(+3 * t_pow_3 - 6 * t_pow_2 + 0 * t + 4) * p2 +
			(-3 * t_pow_3 + 3 * t_pow_2 + 3 * t + 1) * p3 +
			(+1 * t_pow_3 + 0 * t_pow_2 + 0 * t + 0) * p4
			) / 6.0f);
}

void bsplineInternalExternal() {
	int size = center_array.size();
	float CURVE_DISTANCE = 20.0f;

	for (int i = 0; i < size - 3; i += 3) {
		float Ax = center_array[(i + 0) % size];
		float Ay = center_array[(i + 1) % size];
		float Ac = center_array[(i + 2) % size];
		float Bx = center_array[(i + 3) % size];
		float By = center_array[(i + 4) % size];
		float Bc = center_array[(i + 5) % size];

		float w = Bx - Ax;
		float h = By - Ay;
		float a = atan(h / w);

		float internalAngle, externalAngle;

		if (w < 0) {
			internalAngle = a + M_PI / 2;
			externalAngle = a - M_PI / 2;
		}
		else {
			internalAngle = a - M_PI / 2;
			externalAngle = a + M_PI / 2;
		}

		float internalCx = cos(internalAngle) * CURVE_DISTANCE + Ax;
		float internalCy = sin(internalAngle) * CURVE_DISTANCE + Ay;

		internal_array.push_back(internalCx);
		internal_array.push_back(internalCy);
		internal_array.push_back(Ac);

		float externalCx = cos(externalAngle) * CURVE_DISTANCE + Ax;
		float externalCy = sin(externalAngle) * CURVE_DISTANCE + Ay;

		external_array.push_back(externalCx);
		external_array.push_back(externalCy);
		external_array.push_back(Ac);
	}

	glBindVertexArray(VAO_INTERNAL);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_INTERNAL);
	glBufferData(GL_ARRAY_BUFFER, internal_array.size() * sizeof(float), internal_array.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO_EXTERNAL);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_EXTERNAL);
	glBufferData(GL_ARRAY_BUFFER, external_array.size() * sizeof(float), external_array.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
}

void generateTrackObj() {
	bsplineCenter();	
	trackWriter.GenerateTrackObj(points, center_array, internal_array, external_array);
}

void validatePointClick(double posX, double posY) {
	for (int i = 0; i < points.size(); i += 3) {
		float x = points[i];
		float y = points[i + 1];
		float z = points[i + 2];
		int zPos = i + 2;

		float xMin = x - 5.0f;
		float xMax = x + 5.0f;
		float yMin = y - 5.0f;
		float yMax = y + 5.0f;

		if (posX <= xMax && posX >= xMin && posY >= yMin && posY <= yMax) {
			cout << endl << "Ponto de controle: " << i / 3;
			cout << " - Valor = " << z << endl;
			lastPoint = zPos;
			isPointSelected = true;
		}
	}
}

void restart() {
	drawTrack = false;
	points.clear();
	center_array.clear();
	internal_array.clear();
	external_array.clear();
}