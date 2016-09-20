// Local headers
#include "gloom/gloom.hpp"
#include "program.hpp"

// System headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard headers
#include <cstdlib>


// A callback which allows GLFW to report errors whenever they occur.
static void glfwErrorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW returned an error:\n\t%s (%i)\n", description, error);
}


GLFWwindow* initialise()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Could not start GLFW\n");
        exit(EXIT_FAILURE);
    }

    // Set core window options (adjust version numbers if needed)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Enable the GLFW runtime error callback function defined previously.
    glfwSetErrorCallback(glfwErrorCallback);

    // Set additional window options
    glfwWindowHint(GLFW_RESIZABLE, mResizable);
    glfwWindowHint(GLFW_SAMPLES, mSamples);  // MSAA

    // Create window using GLFW
    GLFWwindow* mWindow = glfwCreateWindow(mWidth,
                                           mHeight,
                                           mTitle.c_str(),
                                           nullptr,
                                           nullptr);

    // Ensure the window is set up correctly
    if (!mWindow)
    {
        fprintf(stderr, "Could not open GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Let the window be the current OpenGL context and initialise glad
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();

    // Print various OpenGL information to stdout
    printf("%s: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
    printf("GLFW\t %s\n", glfwGetVersionString());
    printf("OpenGL\t %s\n", glGetString(GL_VERSION));
    printf("GLSL\t %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return mWindow;
}


int main(int argc, char* argb[])
{
    // Initialise window using GLFW
    GLFWwindow* mWindow = initialise();

    // Run an OpenGL application using this window
    runProgram(mWindow);

    // Terminate GLFW (no need to call glfwDestroyWindow)
    glfwTerminate();

    return EXIT_SUCCESS;
}
