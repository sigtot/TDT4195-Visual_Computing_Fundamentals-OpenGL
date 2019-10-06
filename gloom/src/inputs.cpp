#include <GLFW/glfw3.h>
#include <lib/sceneGraph.hpp>
#include "inputs.hpp"

#define TRANS_SPEED 1.0f
#define ROT_SPEED 0.03f

void handleInputsHeli(GLFWwindow* window, SceneNode* sceneNode)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        sceneNode->position.x -= std::sin(sceneNode->rotation.x) * TRANS_SPEED;
        sceneNode->position.z -= std::cos(sceneNode->rotation.x) * TRANS_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        sceneNode->position.x += std::sin(sceneNode->rotation.x) * TRANS_SPEED;
        sceneNode->position.z += std::cos(sceneNode->rotation.x) * TRANS_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        sceneNode->position.y -= TRANS_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        sceneNode->position.y += TRANS_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        sceneNode->rotation.x -= ROT_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        sceneNode->rotation.x += ROT_SPEED;
    }
}

void handleInputsCamera(GLFWwindow* window, Camera &cam)
{
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
        cam = Camera{1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, false};
    }
}

void handleInputsOther(GLFWwindow * window, Camera &cam)
{
    // Use escape key for terminating the GLFW window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        cam.chase = !cam.chase;
    }
}
