// Local headers
#include <gloom/shader.hpp>
#include <vector>
#include "program.hpp"
#include "gloom/gloom.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define NUM_COORDINATES 3
#define NUM_COLOR_COORDINATES 4
#define FOV 90.0f
#define ASPECT_RATIO (16.0f/9.0f)

unsigned int createVAO(std::vector<float> vertices, std::vector<unsigned int> indices, std::vector<float> colors, unsigned int numPoints)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    int num_vbos = 2;
    unsigned int VBO[2];
    unsigned int vertexIndex = 0;
    unsigned int colorIndex = 1;
    glGenBuffers(num_vbos, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[vertexIndex]);
    glBufferData(GL_ARRAY_BUFFER, NUM_COORDINATES * numPoints * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(
            vertexIndex,
            NUM_COORDINATES,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr);
    glEnableVertexAttribArray(vertexIndex);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[colorIndex]);
    glBufferData(GL_ARRAY_BUFFER, NUM_COLOR_COORDINATES * numPoints * sizeof(float), &colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(colorIndex, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(colorIndex);
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
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Enable transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up scene
    Gloom::Shader shader;
    // Fix these dumb paths some time
    shader.makeBasicShader("../gloom/shaders/simple.vert",
                           "../gloom/shaders/simple.frag");
    std::vector<float> triangleCoords {
        0.1, -0.1, -4.0,
        0.1, 0.4, -4.0,
        -0.4, -0.1, -4.0,

        0.2, -0.2, -3.0,
        0.2, 0.3, -3.0,
        -0.3, -0.2, -3.0,

        0.3, -0.3, -2.0,
        0.3, 0.2, -2.0,
        -0.2, -0.3, -2.0,
    };

    std::vector<float> triangleColors {
            0.9, 0.1, 0.3, 0.5, // Red
            0.9, 0.1, 0.3, 0.5, // Red
            0.9, 0.1, 0.3, 0.5, // Red

            0.1, 0.8, 0.0, 0.5, // Green
            0.1, 0.8, 0.0, 0.5, // Green
            0.1, 0.8, 0.0, 0.5, // Green

            0.1, 0.1, 0.9, 0.5, // Blue
            0.1, 0.1, 0.9, 0.5, // Blue
            0.1, 0.1, 0.9, 0.5, // Blue
    };
    std::vector<unsigned int> triangleIndices;
    for (unsigned long i = 0; i * NUM_COORDINATES < triangleCoords.size(); ++i) triangleIndices.push_back(i);
    unsigned int numTrianglePoints = triangleCoords.size() / NUM_COORDINATES;
    unsigned int triangleVAO = createVAO(triangleCoords, triangleIndices, triangleColors, numTrianglePoints);

    glPointSize(5.0f);
    glLineWidth(5.0f);

    int frameNum = 0;
    GLint uniformLoc = shader.getUniformLocation("t_mat");
    if (uniformLoc == -1) {
        throw std::runtime_error("Could not find uniform location");
    }

    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(frameNum % 360)), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(frameNum % 360)), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 perspective = glm::perspective(glm::radians(FOV), ASPECT_RATIO, 1.0f, 100.0f);
        glm::mat4 t_mat = rotateX * rotateY * perspective;
        shader.activate();

        glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(t_mat));

        glBindVertexArray(triangleVAO);
        glDrawElements(GL_TRIANGLES, numTrianglePoints, GL_UNSIGNED_INT, nullptr);

        shader.deactivate();
        printGLError();

        // Handle other events
        glfwPollEvents();
        handleKeyboardInput(window);

        // Flip buffers
        glfwSwapBuffers(window);
        frameNum++;
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
