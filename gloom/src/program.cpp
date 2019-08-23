// Local headers
#include <gloom/shader.hpp>
#include "program.hpp"
#include "gloom/gloom.hpp"

#define NUM_COORDINATES 3

unsigned int createTriangleVAO(float* vertices, unsigned int* indices, int numPoints)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_COORDINATES * numPoints * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(
            0,
            NUM_COORDINATES,
            GL_FLOAT,
            false,
            12,
            nullptr);
    glEnableVertexAttribArray(0);

    // IBO
    unsigned int IBO = 0;
    glGenBuffers(1, &IBO );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numPoints * NUM_COORDINATES * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    return VAO;
}

void runProgram(GLFWwindow* window)
{
    // Enable depth (Z) buffer (accept "closest" fragment)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Configure miscellaneous OpenGL settings
    glEnable(GL_CULL_FACE);

    // Set default colour after clearing the colour buffer
    glClearColor(0.4f, 0.2f, 0.2f, 1.0f);

    // Set up scene
    Gloom::Shader shader;
    shader.makeBasicShader("../gloom/shaders/simple.vert",
                           "../gloom/shaders/simple.frag");
    // A nice triangle
    //float vertices[9] =  { 0.6, 0.7, -1.2, 0.f, 0.4, 0.f, -0.8, -0.2, 1.2 };
    float vertices[9] = {-0.3, -0.4, 0.0, 0.3, -0.1, -0.5, -0.2, 0.5, 0.7};
    unsigned int indices[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int numPoints = 3;
    unsigned int VAO = createTriangleVAO(vertices, indices, numPoints);

    glPointSize(5.0f);
    glLineWidth(5.0f);
    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        shader.activate();
        glDrawElements(GL_TRIANGLES, numPoints, GL_UNSIGNED_INT, nullptr);
        shader.deactivate();
        printGLError();

        // Handle other events
        glfwPollEvents();
        handleKeyboardInput(window);

        // Flip buffers
        glfwSwapBuffers(window);
    }
    shader.destroy();
}


void handleKeyboardInput(GLFWwindow* window)
{
    // Use escape key for terminating the GLFW window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}
