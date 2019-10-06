// Local headers
#include <gloom/shader.hpp>
#include <vector>
#include "program.hpp"
#include "gloom/gloom.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "lib/mesh.hpp"
#include "lib/OBJLoader.hpp"
#include "lib/toolbox.hpp"
#include "inputs.hpp"
#include "vao.hpp"

#define FOV 40.0f
#define ASPECT_RATIO (16.0f/9.0f)

#define Z_FAR_PLANE 10000.0f
#define Z_NEAR_PLANE 1.0f

#define MAIN_ROTOR_SPEED 20.0f
#define TAIL_ROTOR_SPEED 5.0f

#define HELI_TIME_OFFSET 1.6f

#define CHASE_RADIUS 20.0f
#define CHASE_SPEED 0.02f

void spinEntity(SceneNode* rootNode, float speed, double elapsedTime, bool aboutX)
{
    float step = speed * static_cast<float>(elapsedTime);
    if (aboutX) {
        rootNode->rotation.x += step;
    } else {
        rootNode->rotation.y += step;
    }
}

void spinMainRotor(AnimatedNode node, double elapsedTime)
{
    spinEntity(node.sceneNode, MAIN_ROTOR_SPEED, elapsedTime, true);
}

void spinTailRotor(AnimatedNode node, double elapsedTime)
{
    spinEntity(node.sceneNode, TAIL_ROTOR_SPEED, elapsedTime, false);
}

void heliFlyFigureEight(AnimatedNode node, double elapsedTime)
{
    Heading heading = simpleHeadingAnimation(node.time);
    node.sceneNode->position.x = heading.x;
    node.sceneNode->position.z = heading.z;
    node.sceneNode->rotation = glm::vec3(heading.yaw, heading.pitch, heading.roll);
}

SceneNode * addHelicopterNode(SceneNode *&parentNode, std::vector<AnimatedNode> &animated)
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

    heliNode->children = {doorNode, tailRotorNode, mainRotorNode};

    AnimatedNode mainRotorAnimatedNode = AnimatedNode{mainRotorNode, 0.0f, spinMainRotor};
    AnimatedNode tailRotorAnimatedNode = AnimatedNode{tailRotorNode, 0.0f, spinTailRotor};
    animated.push_back(mainRotorAnimatedNode);
    animated.push_back(tailRotorAnimatedNode);

    parentNode->children.push_back(heliNode);

    return heliNode;
}

void createSceneGraph(SceneNode *&rootNode, std::vector<AnimatedNode> &animated)
{
    Mesh lunarSurface = loadTerrainMesh("../gloom/src/resources/lunarsurface.obj");
    SceneNode* terrainNode = createSceneNode();
    terrainNode->vertexArrayObjectID = static_cast<int>(VAOFromMesh(lunarSurface));
    terrainNode->VAOIndexCount = lunarSurface.indices.size();

    // Add five figure-8 flying helicopters
    for (int i = 0; i < 5; i++) {
        SceneNode * heliNode = addHelicopterNode(terrainNode, animated);
        AnimatedNode heliAnimatedNode = AnimatedNode{heliNode, HELI_TIME_OFFSET * static_cast<float>(i), heliFlyFigureEight};
        animated.push_back(heliAnimatedNode);
    }

    rootNode = createSceneNode();
    rootNode->children = {terrainNode};
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

void drawSceneGraph(SceneNode* sceneNode, glm::mat4 viewProjection, GLint tMatUniformLoc, GLint modelMatUniformLoc)
{
    glm::mat4 tMat = viewProjection * sceneNode->currentTransformationMatrix;
    if (sceneNode->vertexArrayObjectID != -1) {
        glUniformMatrix4fv(tMatUniformLoc, 1, GL_FALSE, glm::value_ptr(tMat));
        glUniformMatrix4fv(modelMatUniformLoc, 1, GL_FALSE, glm::value_ptr(sceneNode->currentTransformationMatrix));
        glBindVertexArray(sceneNode->vertexArrayObjectID);
        glDrawElements(GL_TRIANGLES, sceneNode->VAOIndexCount, GL_UNSIGNED_INT, nullptr);
    }

    for (SceneNode* childNode : sceneNode->children) {
        drawSceneGraph(childNode, viewProjection, tMatUniformLoc, modelMatUniformLoc);
    }
}

float control(float x, float ref, float rad)
{
    return CHASE_SPEED * (x - glm::sign(x - ref) * rad - ref);
}

void chase(Camera &cam, const SceneNode *sceneNode)
{
    cam.x -= control(cam.x, sceneNode->position.x, CHASE_RADIUS);
    cam.y -= control(cam.y, sceneNode->position.y, CHASE_RADIUS);
    cam.z -= control(cam.z, sceneNode->position.z, CHASE_RADIUS);
}

void runProgram(GLFWwindow* window)
{
    // Enable depth (Z) buffer (GL_LESS = accept "closest" fragment)
    glEnable(GL_DEPTH_TEST);

    // Enable face culling
    glEnable(GL_CULL_FACE);

    // Set default colour after clearing the colour buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Set up scene
    Gloom::Shader shader;
    // Fix these dumb paths some time
    shader.makeBasicShader("../gloom/shaders/simple.vert",
                           "../gloom/shaders/simple.frag");

    SceneNode* sceneGraph = nullptr;
    std::vector<AnimatedNode> animatedNodes;
    createSceneGraph(sceneGraph, animatedNodes);
    SceneNode* mainHeli = addHelicopterNode(sceneGraph, animatedNodes);
    mainHeli->position.y = 20.0f;

    glPointSize(5.0f);
    glLineWidth(5.0f);

    GLint tMatUniformLoc = shader.getUniformLocation("t_mat");
    GLint modelMatUniformLoc = shader.getUniformLocation("model_mat");
    if (tMatUniformLoc == -1 || modelMatUniformLoc == -1) {
        throw std::runtime_error("Could not find uniform locations");
    }

    Camera cam = Camera{1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, false};
    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 viewMatrix;
        if (cam.chase) {
            viewMatrix = glm::lookAt(glm::vec3(cam.x, cam.y, cam.z), mainHeli->position, glm::vec3(0.0f, 1.0f, 0.0f));
            handleInputsHeli(window, mainHeli);
            chase(cam, mainHeli);
        } else {
            handleInputsCamera(window, cam);
            glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(cam.x, cam.y, cam.z));
            glm::mat4 rotateY = glm::rotate(cam.phi, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation around y
            glm::mat4 rotateX = glm::rotate(cam.theta, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotation around x
            glm::mat4 rotateZ = glm::rotate(cam.psi, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotation around z

            viewMatrix = rotateX * rotateY * rotateZ * translate;
        }

        glm::mat4 perspective = glm::perspective(glm::radians(FOV), ASPECT_RATIO, Z_NEAR_PLANE, Z_FAR_PLANE);
        glm::mat4 tMat = perspective * viewMatrix;
        shader.activate();

        double elapsedTime = getTimeDeltaSeconds();
        for (AnimatedNode &node : animatedNodes) {
            node.time += elapsedTime;
            node.update(node, elapsedTime);
        }
        updateSceneNode(sceneGraph, glm::mat4(1.0f));
        drawSceneGraph(sceneGraph, tMat, tMatUniformLoc, modelMatUniformLoc);

        shader.deactivate();
        printGLError();

        // Handle other events
        glfwPollEvents();
        handleInputsOther(window, cam);

        // Flip buffers
        glfwSwapBuffers(window);
    }
    shader.destroy();
}
