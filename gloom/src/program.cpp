// Local headers
#include <gloom/shader.hpp>
#include <vector>
#include "program.hpp"
#include "gloom/gloom.hpp"

#define NUM_COORDINATES 3

unsigned int createVAO(std::vector<float> vertices, std::vector<unsigned int> indices, unsigned int numPoints)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_COORDINATES * numPoints * sizeof(float), &vertices[0], GL_STATIC_DRAW);

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numPoints * NUM_COORDINATES * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    return VAO;
}

void runProgram(GLFWwindow* window)
{
    // Enable depth (Z) buffer (GL_LESS = accept "closest" fragment)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // This is the default: Can be removed

    // Enable face culling
    glEnable(GL_CULL_FACE);

    // Set default colour after clearing the colour buffer
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Set up scene
    Gloom::Shader shader;
    // Fix these dumb paths some time
    shader.makeBasicShader("/home/sigtot/Developer/visdat/ovs/ov1/gloom/gloom/shaders/simple.vert",
                           "/home/sigtot/Developer/visdat/ovs/ov1/gloom/gloom/shaders/simple.frag");
    std::vector<float> triangleCoords {
        -0.1, 0.0, 0.0, -0.1, 0.5, 0.0, -0.7, 0.0, 0.0,
        0.7, 0.0, 0.0, 0.7, 0.5, 0.0, 0.1, 0.0, 0.0,
    };

    std::vector<unsigned int> indices;
    for (unsigned long i = 0; i * NUM_COORDINATES < triangleCoords.size(); ++i) indices.push_back(i);

    unsigned int numPoints = triangleCoords.size() / NUM_COORDINATES;
    unsigned int VAO = createVAO(triangleCoords, indices, numPoints);

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
