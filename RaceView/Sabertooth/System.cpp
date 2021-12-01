#include <cstdlib>
#include "System.h"
#include "Mesh.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void validateKeys(GLFWwindow* window);
bool checkCollision(glm::vec3 cubePosition, glm::vec3 shotPosition);
void drawCubes(unsigned int VAO1, glm::vec3  cubePositions[12], Shader coreShader);
void handleShot(Shader coreShader);

glm::vec3 positionCamera = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yawa = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitcha = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0 / 2.0f;
float fov = 45.0f;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool hasShot = false;
int iterations = 0;
glm::vec3 shotMove = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 lastFrontPosition = glm::vec3(0.0f, 0.0f, 0.0f);

int cont = 0;

glm::vec3 lightPosition(0.0f, 2.2f, 0.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

int positionTrack = 0;

System::System() {}

System::~System() {}

GLFWwindow* System::GLFWInit()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "FPS_Game", nullptr, nullptr);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();

		return 0;
	}
	glfwMakeContextCurrent(window);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

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
	lightShader = Shader("Shaders/Core/core2.vert", "Shaders/Core/core2.frag");
	coreShader.Use();

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	return window;
}

void System::Run(GLFWwindow* window, GLuint VAO, GLuint VAOlight, glm::vec3 objectsPositions[], glm::vec3 cubePositions[], vector<Mesh*> meshs, vector<vec3*> objPoints)
{
	coreShader.Use();

	for (Mesh* mesh : meshs) {
		for (Group* group : mesh->getGroups()) {

			Material* material = mesh->getMaterial(group->getMaterial());
			coreShader.LoadTexture(_strdup(material->getTexture().c_str()), "texture1", group->getName());
			coreShader.setVec3("materialAmbient", vec3(material->getAmbient()->x, material->getAmbient()->y, material->getAmbient()->z));
			coreShader.setVec3("materialDiffuse", vec3(material->getDiffuse()->x, material->getDiffuse()->y, material->getDiffuse()->z));
			coreShader.setVec3("materialSpecular", vec3(material->getSpecular()->x, material->getSpecular()->y, material->getSpecular()->z));
			coreShader.setFloat("materialShininess", material->getShininess());

			vector<float> vertices;
			vector<float> normais;
			vector<float> textures;

			for (Face* face : group->getFaces()) {
				for (int verticeID : face->getVertices()) {
					glm::vec3* vertice = mesh->vertice(verticeID - 1);
					vertices.push_back(vertice->x);
					vertices.push_back(vertice->y);
					vertices.push_back(vertice->z);

					group->increaseNumVertices();
				}

				for (int normalID : face->getNormais()) {
					glm::vec3* normal = mesh->normal(normalID - 1);
					normais.push_back(normal->x);
					normais.push_back(normal->y);
					normais.push_back(normal->z);
				}

				for (int textureID : face->getTextures()) {
					glm::vec2* texture = mesh->texture(textureID - 1);
					textures.push_back(texture->x);
					textures.push_back(texture->y);
				}
			}

			GLuint VBOvertices, VBOnormais, VBOtextures, VAO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBOvertices);
			glGenBuffers(1, &VBOnormais);
			glGenBuffers(1, &VBOtextures);			

			glBindVertexArray(VAO);
			
			glBindBuffer(GL_ARRAY_BUFFER, VBOvertices);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			
			glBindBuffer(GL_ARRAY_BUFFER, VBOnormais);
			glBufferData(GL_ARRAY_BUFFER, normais.size() * sizeof(float), normais.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			
			glBindBuffer(GL_ARRAY_BUFFER, VBOtextures);
			glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(float), textures.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(2);

			group->setVAO(&VAO);
			glBindVertexArray(0); // Unbind VAO
		}
	}
	// game loop
	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		validateKeys(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		coreShader.Use();

		glm::mat4 view(1.0f);
		view = glm::lookAt(positionCamera, positionCamera + cameraFront, cameraUp);
		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
		coreShader.setMatrix4fv("projection", proj);
		coreShader.setMatrix4fv("view", view);
		coreShader.setVec3("ligthDiff", glm::vec3(0.5f, 0.5f, 0.5f));
		coreShader.setVec3("ligthSpec", glm::vec3(1.0f, 1.0f, 1.0f));
		coreShader.setVec3("lightColor", lightColor);
		coreShader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
		coreShader.setVec3("viewPos", positionCamera);

		glm::mat4 model(1.0f);
		coreShader.setMatrix4fv("model", model);
		glUniform1i(glGetUniformLocation(coreShader.program, "tex"), 0);
		for (Mesh* mesh : meshs) {
			for (Group* group : mesh->getGroups()) {				
				coreShader.UseTexture(group->getName());
				glBindVertexArray(group->getVAO());
				glDrawArrays(GL_TRIANGLES, 0, group->getNumVertices());
				glBindVertexArray(0);
			}
		}

		drawCubes(VAO, cubePositions, coreShader);

		lightShader.Use();
		lightShader.setMatrix4fv("projection", proj);
		lightShader.setMatrix4fv("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		//model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		//lightShader.setMatrix4fv("model", model);
		glUniform1i(glGetUniformLocation(coreShader.program, "tex"), 1);
		//coreShader.Use();
		glBindVertexArray(VAOlight);

		// calculate the model matrix for each object and pass it to shader before drawing
		//glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::translate(model, cubePositions[0]);
		lightShader.setMatrix4fv("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//car
		coreShader.Use();
		glUniform1i(glGetUniformLocation(coreShader.program, "tex"), 2);
		glBindVertexArray(VAO);
		model = glm::mat4(1.0f);
		if (positionTrack == objPoints.size())
			positionTrack = 0;

		int aux;
		if (positionTrack == objPoints.size() - 1) {
			aux = 0;
		}
		else
		{
			aux = positionTrack + 1;
		}
		glm::vec3 a = (*objPoints[positionTrack]);
		glm::vec3 b = (*objPoints[aux]);

		GLfloat dx = b.x - a.x;
		GLfloat dy = b.y - a.y;
		GLfloat dz = b.z - a.z;

		GLfloat angle = -glm::atan(dz, dx);
		GLfloat angleY = -glm::atan(dx, dy);

		model = translate(model, *objPoints[positionTrack++]);
		model = rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		model = rotate(model, angleY, glm::vec3(0.0f, 0.0f, 1.0f));
		model = scale(model, glm::vec3(0.5, 0.5, 0.5));

		coreShader.setMatrix4fv("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		handleShot(coreShader);

		glfwSwapBuffers(window);
	}
}

void handleShot(Shader coreShader)
{		
	if (hasShot) {
		if (iterations == 0) {
			glm::vec3 temp = positionCamera;
			temp[1] = 1.0f;
			shotMove = temp;
			lastFrontPosition = cameraFront;
		}

		iterations += 1;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, shotMove);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		float shotSpeed = 80 * deltaTime;
		shotMove += shotSpeed * lastFrontPosition;
		coreShader.setMatrix4fv("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		if (iterations >= 200) {
			hasShot = false;
			iterations = 0;
		}
	}
}

void drawCubes(unsigned int VAO1, glm::vec3  cubePositions[12], Shader coreShader)
{
	//------------------Desenha cubos---------------------------------
	// Carrega cor dos cubos
	glUniform1i(glGetUniformLocation(coreShader.program, "tex"), 1);
	coreShader.Use();
	glBindVertexArray(VAO1);
	for (unsigned int i = 0; i < 12; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		coreShader.setMatrix4fv("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	//----------------------------------------------------------------
}

// Valida todas as teclas pressionadas
// ---------------------------------------------------------------------------------------------------------
void validateKeys(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//Trata movimento do "personagem"
	float cameraSpeed = 7 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		positionCamera += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		positionCamera -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		positionCamera -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		positionCamera += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	//Trata clique do mouse para o tiro
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		hasShot = true;
}

void System::Finish(GLuint VAO, GLuint VAO1)
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VAO1);
	coreShader.Delete();
	lightShader.Delete();
	glfwTerminate();
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.4f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yawa += xoffset;
	pitcha += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitcha > 89.0f)
		pitcha = 89.0f;
	if (pitcha < -89.0f)
		pitcha = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yawa)) * cos(glm::radians(pitcha));
	front.y = sin(glm::radians(pitcha));
	front.z = sin(glm::radians(yawa)) * cos(glm::radians(pitcha));
	cameraFront = glm::normalize(front);
}

// chech collision
bool checkCollision(glm::vec3 cubePosition, glm::vec3 shotPosition) {
	float cubeX = cubePosition[0];
	float cubeZ = cubePosition[2];
	float shotX = shotPosition[0];
	float shotZ = shotPosition[2];

	int diffX = std::abs(cubeX - shotX);
	int diffZ = std::abs(cubeZ - shotZ);

	if ((diffX < 0.1f && diffX > -0.1f) || (diffZ < 0.1f && diffZ > -0.1f)) {
		return true;
	}
	else {
		return false;
	}
}