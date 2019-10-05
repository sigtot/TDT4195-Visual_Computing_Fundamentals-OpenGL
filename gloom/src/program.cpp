// Local headers
#include <cmath>
#include <gloom/shader.hpp>
#include <vector>
#include "program.hpp"
#include "gloom/gloom.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "lib/mesh.hpp"
#include "lib/OBJLoader.hpp"
#include "lib/sceneGraph.hpp"


#define NUM_COORDINATES 3
#define NUM_COLOR_COORDINATES 4
#define FOV 40.0f
#define ASPECT_RATIO (16.0f/9.0f)
#define TRANS_SPEED 1.0f
#define ROT_SPEED 0.03f

#define Z_FAR_PLANE 10000.0f
#define Z_NEAR_PLANE 1.0f

unsigned int createVAO(
        std::vector<float> vertices,
        std::vector<unsigned int> indices,
        std::vector<float> colors,
        std::vector<float> normals,
        unsigned int numPoints)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    int num_vbos = 3;
    unsigned int VBO[3];
    unsigned int vertexIndex = 0;
    unsigned int colorIndex = 1;
    unsigned int normalIndex = 2;
    glGenBuffers(num_vbos, VBO);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO[vertexIndex]);
    glBufferData(GL_ARRAY_BUFFER, NUM_COORDINATES * numPoints * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(vertexIndex, NUM_COORDINATES, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(vertexIndex);

    // Colors
    glBindBuffer(GL_ARRAY_BUFFER, VBO[colorIndex]);
    glBufferData(GL_ARRAY_BUFFER, NUM_COLOR_COORDINATES * numPoints * sizeof(float), &colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(colorIndex, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(colorIndex);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[normalIndex]);
    glBufferData(GL_ARRAY_BUFFER, NUM_COORDINATES * numPoints * sizeof(float), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(normalIndex, NUM_COORDINATES, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(normalIndex);

    // IBO
    unsigned int IBO = 0;
    glGenBuffers(1, &IBO );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_COORDINATES * numPoints * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    return VAO;
}

unsigned int VAOFromMesh(Mesh mesh)
{
    return createVAO(
        mesh.vertices,
        mesh.indices,
        mesh.colours,
        mesh.normals,
        mesh.vertexCount());
}

SceneNode* createSceneGraph()
{
    Helicopter heli = loadHelicopterModel("../gloom/src/resources/helicopter.obj");
    SceneNode* heliNode = createSceneNode();
    heliNode->vertexArrayObjectID = static_cast<int>(VAOFromMesh(heli.body));
    heliNode->VAOIndexCount = heli.body.indices.size();

    SceneNode* doorNode = createSceneNode();
    doorNode->vertexArrayObjectID = static_cast<int>(VAOFromMesh(heli.door));
    doorNode->VAOIndexCount = heli.door.indices.size();

    SceneNode* tailRotorNode = createSceneNode();
    tailRotorNode->vertexArrayObjectID = static_cast<int>(VAOFromMesh(heli.tailRotor));
    tailRotorNode->VAOIndexCount = heli.tailRotor.indices.size();
    tailRotorNode->referencePoint = glm::vec3(0.35f, 2.3f, 10.4f);

    SceneNode* mainRotorNode = createSceneNode();
    mainRotorNode->vertexArrayObjectID = static_cast<int>(VAOFromMesh(heli.mainRotor));
    mainRotorNode->VAOIndexCount = heli.mainRotor.indices.size();

    Mesh lunarSurface = loadTerrainMesh("../gloom/src/resources/lunarsurface.obj");
    SceneNode* terrainNode = createSceneNode();
    terrainNode->vertexArrayObjectID = static_cast<int>(VAOFromMesh(lunarSurface));
    terrainNode->VAOIndexCount = lunarSurface.indices.size();

    SceneNode* rootNode = createSceneNode();

    heliNode->children = {doorNode, tailRotorNode, mainRotorNode};
    terrainNode->children = {heliNode};
    rootNode->children = {terrainNode};
    return rootNode;
}

glm::mat4 rotateAroundPoint(glm::vec3 rot, glm::vec3 referencePoint)
{
    glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 translateFromRef = glm::translate(identity, referencePoint);
    glm::mat4 rotateX = glm::rotate(rot.x, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotateY = glm::rotate(rot.y, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotateZ = glm::rotate(rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 translateToRef = glm::translate(identity, -referencePoint);

    return translateFromRef * rotateZ * rotateY * rotateX * translateToRef;
}

void updateSceneNode(SceneNode* sceneNode, glm::mat4 transformationThusFar)
{
    sceneNode->currentTransformationMatrix =
            transformationThusFar
            * glm::translate(sceneNode->position)
            * rotateAroundPoint(sceneNode->rotation, sceneNode->referencePoint);

    for (SceneNode* childNode : sceneNode->children) {
        updateSceneNode(childNode, sceneNode->currentTransformationMatrix);
    }
}

void drawSceneGraph(SceneNode* sceneNode, glm::mat4 viewProjection, GLint tMatUniformLoc)
{
    glm::mat4 tMat = viewProjection * sceneNode->currentTransformationMatrix;
    if (sceneNode->vertexArrayObjectID != -1) {
        glUniformMatrix4fv(tMatUniformLoc, 1, GL_FALSE, glm::value_ptr(tMat));
        glBindVertexArray(sceneNode->vertexArrayObjectID);
        glDrawElements(GL_TRIANGLES, sceneNode->VAOIndexCount, GL_UNSIGNED_INT, nullptr);
    }

    for (SceneNode* childNode : sceneNode->children) {
        drawSceneGraph(childNode, viewProjection, tMatUniformLoc);
    }
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

    SceneNode* sceneGraph = createSceneGraph();

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
        glm::mat4 perspective = glm::perspective(glm::radians(FOV), ASPECT_RATIO, Z_NEAR_PLANE, Z_FAR_PLANE);
        glm::mat4 tMat = perspective * transform;
        shader.activate();

        updateSceneNode(sceneGraph, glm::mat4(1.0f));
        drawSceneGraph(sceneGraph, tMat, uniformLoc);

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
