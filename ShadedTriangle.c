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
	GLFWwindow* window = glfwCreateWindow(480, 480, "Simple Triangle", NULL, NULL);

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
	float triangleColors[9] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	// Generate and Populate a Vertex Buffer Object (VBO) for points
	GLuint pointsVBO_ID;
	glGenBuffers(1, &pointsVBO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO_ID);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triangleVertices, GL_STATIC_DRAW);

	// Generate and Populate a VBO for vertex colors
	GLuint colorsVBO_ID;
	glGenBuffers(1, &colorsVBO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, colorsVBO_ID);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), triangleColors, GL_STATIC_DRAW);

	// Generate and Bind a Vertex Array Object (VAO) to the VBO
	GLuint vaoIdentifier;
	glGenVertexArrays(1, &vaoIdentifier);
	glBindVertexArray(vaoIdentifier);
	
	// Add attributes to the VAO
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO_ID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colorsVBO_ID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Enable the attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Load vertex shader script
	char vertShaderBuffer[256] = {'\0'};
	FILE* vertFile = fopen("shadedTriangle.vert", "r");
	if (!vertFile) {
		perror("shadedTriangle.vert could not be opened.");
		exit(EXIT_FAILURE);
	}
	fread(vertShaderBuffer, sizeof(char), 255, vertFile);
	if (strlen(vertShaderBuffer) == 255) {
		perror("Too many characters in shadedTriangle.vert. Increase buffer size.");
		exit(EXIT_FAILURE);
	} else {
		printf("%lu/254 characters used in shadedTriangle.vert.\n", strlen(vertShaderBuffer));
	}
	fclose(vertFile);

	// Load fragment shader script
	char fragShaderBuffer[256] = {'\0'};
	FILE* fragFile = fopen("shadedTriangle.frag", "r");
	if (!fragFile) {
		perror("shadedTriangle.frag could not be opened.");
		exit(EXIT_FAILURE);
	}
	fread(fragShaderBuffer, sizeof(char), 255, fragFile);
	if (strlen(fragShaderBuffer) == 255) {
		perror("Too many characters in shadedTriangle.frag. Increase buffer size.");
		exit(EXIT_FAILURE);
	} else {
		printf("%lu/254 characters used in shadedTriangle.frag.\n", strlen(fragShaderBuffer));
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
	glBindAttribLocation(shaderProgramID, 0, "vertex_position");
	glBindAttribLocation(shaderProgramID, 1, "vertex_color");
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
