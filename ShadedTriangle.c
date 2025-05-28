// Preprocessing Statements
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define EXIT_INIT_FAIL 2
#define EXIT_WINDOW_FAIL 3


// GLFW Error Callback
static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
// GLFW Key Callback
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Quit Key
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}


int main(void) {
	if (!glfwInit()) {
		exit(EXIT_INIT_FAIL);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // MacOS only supports the core profile
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(240, 240, "Simple Triangle", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_WINDOW_FAIL);
	}
	
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval(1); // VSync and caps framerate to monitor refresh rate

	glfwSetErrorCallback(errorCallback);
	glfwSetKeyCallback(window, keyCallback);

	float triangleVertices[9] = {
		 0.0f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	// Generate and Populate a Vertex Buffer Object (VBO)
	GLuint vboIdentifier = 0;
	glGenBuffers(1, &vboIdentifier);
	glBindBuffer(GL_ARRAY_BUFFER, vboIdentifier);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triangleVertices, GL_STATIC_DRAW);

	// Generate and Bind a Vertex Array Object (VAO) to the VBO
	GLuint vaoIdentifier = 0;
	glGenVertexArrays(1, &vaoIdentifier);
	glBindVertexArray(vaoIdentifier);
	glEnableVertexAttribArray(0); // We only have 1 VBO and it will be the 0th attribute
	glBindBuffer(GL_ARRAY_BUFFER, vboIdentifier);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Load vertex shader script
	char vertShaderBuffer[256] = {'\0'};
	FILE* vertFile = fopen("simpleTriangle.vert", "r");
	if (!vertFile) {
		perror("simpleTriangle.vert could not be opened.");
		exit(EXIT_FAILURE);
	}
	fread(vertShaderBuffer, sizeof(char), 255, vertFile);
	if (strlen(vertShaderBuffer) == 255) {
		perror("Too many characters in simpleTriangle.vert. Increase buffer size.");
		exit(EXIT_FAILURE);
	}
	fclose(vertFile);

	// Load fragment shader script
	char fragShaderBuffer[256] = {'\0'};
	FILE* fragFile = fopen("simpleTriangle.frag", "r");
	if (!fragFile) {
		perror("simpleTriangle.frag could not be opened.");
		exit(EXIT_FAILURE);
	}
	fread(fragShaderBuffer, sizeof(char), 255, fragFile);
	if (strlen(fragShaderBuffer) == 255) {
		perror("Too many characters in simpleTriangle.frag. Increase buffer size.");
		exit(EXIT_FAILURE);
	}
	fclose(fragFile);

	// Create and compile a vertex shader
	GLuint vsIdentifier = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vsbPtr = vertShaderBuffer;
	glShaderSource(vsIdentifier, 1, &vsbPtr, NULL);
	glCompileShader(vsIdentifier);

	// Create and compile a fragment shader
	GLuint fsIdentifier = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fsbPtr = fragShaderBuffer;
	glShaderSource(fsIdentifier, 1, &fsbPtr, NULL);
	glCompileShader(fsIdentifier);

	// Create a GPU shader program with the vertex and fragment shaders
	GLuint shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, fsIdentifier);
	glAttachShader(shaderProgramID, vsIdentifier);
	glLinkProgram(shaderProgramID);


	while (!glfwWindowShouldClose(window))
	{ // Draw Loop

		// Clear the surface
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Load the shader program and the VAO
		glUseProgram(shaderProgramID);
		glBindVertexArray(vaoIdentifier);

		// Draw 3 vertices with the loaded shaders
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Update the screen
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Quit
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
