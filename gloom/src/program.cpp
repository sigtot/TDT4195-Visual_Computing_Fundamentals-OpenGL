// Local headers
#include <cmath>
#include <gloom/shader.hpp>
#include <vector>
#include "program.hpp"
#include "gloom/gloom.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>


#define NUM_COORDINATES 3
#define NUM_COLOR_COORDINATES 4
#define FOV 40.0f
#define ASPECT_RATIO (16.0f/9.0f)
#define TRANS_SPEED 0.05f
#define ROT_SPEED 0.03f

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
        0.1, -0.7, -4.0,
        0.1, -0.2, -4.0,
        -0.4, -0.7, -4.0,

        0.2, -0.8, -3.0,
        0.2, -0.3, -3.0,
        -0.3, -0.8, -3.0,

        0.3, -0.9, -2.0,
        0.3, -0.4, -2.0,
        -0.2, -0.9, -2.0,
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

    GLint uniformLoc = shader.getUniformLocation("t_mat");
    if (uniformLoc == -1) {
        throw std::runtime_error("Could not find uniform location");
    }

    Camera cam = Camera{1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f};
    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 identity = glm::mat4(1.0f);
        glm::mat4 translate = glm::translate(identity, glm::vec3(cam.x, cam.y, cam.z));
        glm::mat4 rotateY = glm::rotate(cam.phi, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation around y
        glm::mat4 rotateX = glm::rotate(cam.theta, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotation around x
        glm::mat4 rotateZ = glm::rotate(cam.psi, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotation around z

        glm::mat4 transform = rotateX * rotateY * rotateZ * translate;
        glm::mat4x4 transposedTrans = glm::transpose(transform);
        glm::mat4 perspective = glm::perspective(glm::radians(FOV), ASPECT_RATIO, 1.0f, 100.0f);
        glm::mat4 t_mat = transposedTrans * perspective;
        shader.activate();

        glUniformMatrix4fv(uniformLoc, 1, GL_TRUE, glm::value_ptr(t_mat));

        glBindVertexArray(triangleVAO);
        glDrawElements(GL_TRIANGLES, numTrianglePoints, GL_UNSIGNED_INT, nullptr);

        shader.deactivate();
        printGLError();

        // Handle other events
        glfwPollEvents();
        handleKeyboardInput(window, cam);

        // Flip buffers
        glfwSwapBuffers(window);
    }
    shader.destroy();
}

void handleKeyboardInput(GLFWwindow* window, Camera &cam)
{
    // Use escape key for terminating the GLFW window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam.x += std::cos(cam.phi) * TRANS_SPEED;
        cam.z += std::sin(cam.phi) * TRANS_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam.x -= std::cos(cam.phi) * TRANS_SPEED;
        cam.z -= std::sin(cam.phi) * TRANS_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam.x -= std::sin(cam.phi) * TRANS_SPEED;
        cam.z += std::cos(cam.phi) * TRANS_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam.x += std::sin(cam.phi) * TRANS_SPEED;
        cam.z -= std::cos(cam.phi) * TRANS_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cam.y += TRANS_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cam.y -= TRANS_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        cam.phi += ROT_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        cam.phi -= ROT_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        cam.theta += ROT_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        cam.theta -= ROT_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        cam.psi += ROT_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        cam.psi -= ROT_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        cam = Camera{1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f};
    }
}
