// Local headers
#include <gloom/shader.hpp>
#include <vector>
#include <cmath>
#include "program.hpp"
#include "gloom/gloom.hpp"

#define NUM_COORDINATES 3
#define NUM_COLOR_COORDINATES 4

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

std::vector<float> calcOutputs(const std::vector<float> &inputs, float (*func)(float))
{
    std::vector<float> outputs;
    outputs.reserve(inputs.size());
    for (float val : inputs) {
        outputs.push_back((float)func(val));
    }
    return outputs;
}

std::vector<float> calcCoordsXY(const std::vector<float> &funcInputs, float (*func)(float))
{
    std::vector<float> funcOutputs = calcOutputs(funcInputs, func);
    std::vector<float> vertices;
    for (unsigned long i = 0; i < funcInputs.size(); i++) {
        vertices.push_back(funcInputs[i]);
        vertices.push_back(funcOutputs[i]);
        vertices.push_back(0.0f);
    }
    return vertices;
}

float upperHalfCircle(float x)
{
    return static_cast<float>(sqrt(0.25 - x*x));
}

float lowerHalfCircle(float x)
{
    return static_cast<float>(-sqrt(0.25 - x*x));
}

float squishySine(float x)
{
    return static_cast<float>(std::sin(6*x));
}

template <typename T>
std::vector<T> concatVectors(std::vector<T> A, std::vector<T> B)
{
    std::vector<T> AB;
    AB.reserve( A.size() + B.size() );
    AB.insert( AB.end(), A.begin(), A.end() );
    AB.insert( AB.end(), B.begin(), B.end() );
    return AB;
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
        0.1, -0.1, 0.2,
        0.1, 0.4, 0.2,
        -0.4, -0.1, 0.2,

        0.2, -0.2, 0.1,
        0.2, 0.3, 0.1,
        -0.3, -0.2, 0.1,

        0.3, -0.3, 0.0,
        0.3, 0.2, 0.0,
        -0.2, -0.3, 0.0,
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
    GLint uniformLoc = shader.getUniformLocation("frame_num");
    if (uniformLoc == -1) {
        throw std::runtime_error("Could not find uniform location");
    }

    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.activate();

        glUniform1i(uniformLoc, frameNum);

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
