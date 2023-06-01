#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float zoom_factor = 1, x_shift = 0, y_shift = 0;

GLchar *LoadShader(const std::string &file)
{
    std::fstream shaderFile;
    int shaderFileLength;

    shaderFile.open(file);

    if (shaderFile.fail())
    {
        throw std::runtime_error("COULD NOT FIND SHADER FILE");
    }

    shaderFile.seekg(0, shaderFile.end);
    shaderFileLength = shaderFile.tellg();
    //std::cout << shaderFileLength << std::endl;
    shaderFile.seekg(0, shaderFile.beg);

    GLchar *shaderCode = new GLchar[shaderFileLength + 1];
    shaderFile.read(shaderCode, shaderFileLength);

    shaderFile.close();

    shaderCode[shaderFileLength] = '\0';

    //std::cout << shaderCode << std::endl;

	return shaderCode;
}

glm::mat4 viewmatrix(float x_res, float y_res){
    float scale_factor = 0.25f*y_res*zoom_factor;
    glm::mat4 viewmat = glm::mat4(1.0f);
    viewmat[0] = glm::vec4(1.0f/scale_factor, 0.0f, -0.5f*x_res/scale_factor + x_shift, 0.0f);
    viewmat[1] = glm::vec4(0.0f, 1.0f/scale_factor, -0.5f*y_res/scale_factor + y_shift, 0.0f);
    viewmat[2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    return viewmat;
}

bool mouseOff = false;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
            zoom_factor *= 1.008;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
            zoom_factor /= 1.008;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            y_shift += 0.01/zoom_factor;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            y_shift -= 0.01/zoom_factor;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            x_shift += 0.01/zoom_factor;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            x_shift -= 0.01/zoom_factor;
    if((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && !mouseOff)
        mouseOff = true;
    else if((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && mouseOff)
        mouseOff = false;
    if((glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) && mouseOff)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else if((glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) && !mouseOff)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool returnMouse(){
    return !mouseOff;  
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}