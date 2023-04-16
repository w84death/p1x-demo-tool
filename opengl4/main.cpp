#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shader.h"

int WIDTH = 640, HEIGHT = 360;
float demoTime = 0.0;
const float demoLength = 60.0;
bool isPlaying = true;
bool fullscreen = false;

void _init(void){};
int main(int argc, char* argv[]) {

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--width" && i + 1 < argc) {
            WIDTH = std::stof(argv[++i]);
        } else if (arg == "--height" && i + 1 < argc) {
            HEIGHT = std::stof(argv[++i]);
        }else if (arg == "--fullscreen") {
            fullscreen = true;
        }
    }

    glfwInit();

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "-=[ P1X DEMO TOOL ]=-", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // Set up vertex data
    GLfloat vertices[] = {
        // Positions     // Texture Coords (UV)
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3
    };


    // Set viewport size
    glViewport(0, 0, WIDTH, HEIGHT);


    // Set up vertex buffer and vertex array objects
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates (UV) attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO


    // Load shaders
    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");


    // Uniforms
    GLint timeLocation = glGetUniformLocation(shader.Program, "time");
    GLint widthLocation = glGetUniformLocation(shader.Program, "width");
    GLint heightLocation = glGetUniformLocation(shader.Program, "height");


    float lastTime = static_cast<float>(glfwGetTime());
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the plane
        shader.Use();
        glUniform1f(timeLocation, demoTime);
        glUniform1f(widthLocation, static_cast<float>(WIDTH));
        glUniform1f(heightLocation, static_cast<float>(HEIGHT));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

        float nowTime = static_cast<float>(glfwGetTime());
        float deltaTime = nowTime - lastTime;
        lastTime = nowTime;
        int fps = static_cast<int>(1.0f / deltaTime);
        int frameMs = static_cast<int>(deltaTime * 1000);

        if(isPlaying) demoTime += deltaTime;

        std::cout << "[" << WIDTH << "x" << HEIGHT << "] " << fps << " fps, " << frameMs << " ms" << ", DEMO TIME: " << demoTime << "s" << std::endl;

    }

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();

    return 0;
}
