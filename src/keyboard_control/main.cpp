#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <fmt/base.h>

// Box position
float boxX = 0.0f;
float boxY = 0.0f;
const float boxSize = 0.1f;
const float moveSpeed = 0.01f;

// Shader sources
const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
uniform vec2 uOffset;
void main() {
    gl_Position = vec4(aPos.x + uOffset.x, aPos.y + uOffset.y, 0.0, 1.0);
}
)";

const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red
}
)";

GLuint shaderProgram{};
GLuint VAO{}, VBO{};

void setupShaders() {
	// Compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	// Compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	// Link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Clean up
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void setupBox() {
	// float vertices[] = {
	std::array<float, 8> vertices = {
		-boxSize, -boxSize, // bottom left
		boxSize,  -boxSize, // bottom right
		boxSize,  boxSize,	// top right
		-boxSize, boxSize	// top left
	};

	std::array<std::uint32_t, 8> indices = {
		0, 1, 2, // first triangle
		0, 2, 3	 // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	GLuint EBO{};
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_UP:
			boxY += moveSpeed;
			fmt::println("Up pressed - Box position: ({:.3f}, {:.3f})", boxX, boxY);
			break;
		case GLFW_KEY_DOWN:
			boxY -= moveSpeed;
			fmt::println("Down pressed - Box position: ({:.3f}, {:.3f})", boxX, boxY);
			break;
		case GLFW_KEY_LEFT:
			boxX -= moveSpeed;
			fmt::println("Left pressed - Box position: ({:.3f}, {:.3f})", boxX, boxY);
			break;
		case GLFW_KEY_RIGHT:
			boxX += moveSpeed;
			fmt::println("Right pressed - Box position: ({:.3f}, {:.3f})", boxX, boxY);
			break;
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		}
	}
}

auto main() -> int {
	fmt::println("Hello OpenGL - Arrow Keys to Move Box!");

	if (!glfwInit()) {
		fmt::print(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Modern OpenGL

	GLFWwindow *window = glfwCreateWindow(800, 600, "Moving Box", nullptr, nullptr);
	if (!window) {
		fmt::print(stderr, "Failed to create window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);

	if (glewInit() != GLEW_OK) {
		fmt::print(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	fmt::println("OpenGL version: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
	fmt::println("Use arrow keys to move the red box. Press ESC to exit.");

	setupShaders();
	setupBox();

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), boxX, boxY);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
