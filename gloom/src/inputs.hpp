#ifndef GLOOM_INPUTS_HPP
#define GLOOM_INPUTS_HPP

#include "program.hpp"

// Function for handling key presses
void handleInputsCamera(GLFWwindow* window, Camera &cam);
void handleInputsHeli(GLFWwindow* window, SceneNode* sceneNode);
void handleInputsOther(GLFWwindow* window, Camera &cam);

#endif //GLOOM_INPUTS_HPP
