#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include "implot.h"
#include "implot_internal.h"
#include "shader(alt).h"
#include "computeshader(alt).h"
#include "textures.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"

//backend processes (window and i/o)
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void gui_editor(bool &run_sim, bool &QorC);
bool returnMouse();

//window size
const unsigned int SCR_WIDTH = 3440;
const unsigned int SCR_HEIGHT = 1440;

Camera cam(0.0f, 10.0f, -10.0f);
bool firstMouse = true;
bool classical = true;
bool evolve = true;

//view matrix
glm::mat4 viewmatrix(float x_res, float y_res); 

int main(){
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Wave APP", glfwGetPrimaryMonitor(), NULL);
    cam.setWindow(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSwapInterval(0);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    const int gridX = 1024, gridY = 1024;
    Shader shader("shaders(vf)/v_shader.vert","shaders(vf)/f_shader.frag");

    shader.link_shader("shaders(vf)/post_processing.glsl");
    shader.compile_shader();

    // Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
    ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

    unsigned int render, data, derivative;

    glGenTextures(1, &render);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gridX, gridY, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, render, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    glGenTextures(1, &data);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gridX, gridY, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(1, data, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    glGenTextures(1, &derivative);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, derivative);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gridX, gridY, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(2, derivative, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    
    cShader main("shaders(comp)/wave.comp", gridX, gridY);
    cShader initial("shaders(comp)/initial(wave).comp", gridX, gridY);

    initial.compile_shader();
    main.compile_shader();

    initial.use();
    initial.setFloat("time", 0);

    const int grid_size = 1024; 
    int vSize = grid_size*grid_size*5;
    int iSize = 6*(grid_size-1)*(grid_size-1);

    float *vertices = new float[grid_size*grid_size*5];
    unsigned int *indices = new unsigned int[6*(grid_size-1)*(grid_size-1)];

    if (!vertices)
    {
        std::cout << "Memory allocation failed\n";
    }

    for(int i = 0; i < grid_size; i ++){
        for(int j = 0; j < grid_size; j ++){
            vertices[5*(i*grid_size + j)+0] = (float)(i-grid_size/2);
            vertices[5*(i*grid_size + j)+1] = 0.0f;
            vertices[5*(i*grid_size + j)+2] = (float)(j-grid_size/2);
            vertices[5*(i*grid_size + j)+3] = (float)i/((float)grid_size - 1.0f);
            vertices[5*(i*grid_size + j)+4] = (float)j/((float)grid_size - 1.0f);
        }
    }

    for(int i = 0; i < grid_size - 1; i ++){
        for(int j = 0; j < grid_size - 1; j ++){
            int x = (i+1)*grid_size + (j+1);
            int y = x - 1;
            indices[6*(i*(grid_size-1) + j) + 0] = x;
            indices[6*(i*(grid_size-1) + j) + 1] = x-1;
            indices[6*(i*(grid_size-1) + j) + 2] = x-grid_size;
            indices[6*(i*(grid_size-1) + j) + 3] = y;
            indices[6*(i*(grid_size-1) + j) + 4] = y - grid_size;
            indices[6*(i*(grid_size-1) + j) + 5] = y - grid_size + 1;
        }
    }

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vSize*sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize*sizeof(float), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);

    delete[] vertices;
    delete[] indices;

    glm::mat4 viewspace = viewmatrix(gridX, gridY);
    //initial.use();
    //initial.setMat4("viewmat", viewspace);

    //glm::mat4 model = glm::mat4(1.0f);
    //glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);
    //projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(60.0f), float(SCR_WIDTH) / float(SCR_HEIGHT), 5.1f, 2000000.0f);
    scale = glm::scale(scale, glm::vec3(1.0f));
    glm::mat4 mvp;

    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    int iter = 100;
    //glShadeModel(GL_SMOOTH);
    glEnable(GL_ARB_shader_precision);
    //glEnable(GL_POLYGON_SMOOTH);

    //glEnable (GL_BLEND);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float fake_time = 0;
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        if(returnMouse())
            view = cam.updateView();
        mvp = projection * view * scale;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

        ImGui::Begin("Editor");
        gui_editor(evolve, classical);
        ImGui::End();
        //bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, render);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, data);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, derivative);

        if(evolve){
            main.use();
            main.setFloat("time", fake_time);
            main.setInt("classical", classical);
            //copies new data over to current data
            glCopyImageSubData(derivative, GL_TEXTURE_2D, 0, 0, 0, 0,
                   render, GL_TEXTURE_2D, 0, 0, 0, 0,
                   1024, 1024, 1);
            fake_time += 1.0f;
        }
        else{
            initial.use();
            //main.use();
            main.setInt("classical", classical);
            initial.setFloat("time", 0);
            fake_time = 0.0f;
        }

        glm::vec3 camPos = cam.cameraPos;

        shader.use();
        shader.setVec3("viewPos", camPos[0],camPos[1],camPos[2]);
        shader.setFloat("time", (float)glfwGetTime());
        shader.setMat4("mvp", mvp);
        shader.setInt("render_option", 0);
        shader.setInt("classical", classical);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6*(grid_size-1)*(grid_size-1), GL_UNSIGNED_INT, 0);

        if(!classical){
        shader.use();
        shader.setInt("render_option", 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6*(grid_size-1)*(grid_size-1), GL_UNSIGNED_INT, 0);

        shader.use();
        shader.setInt("render_option", 2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6*(grid_size-1)*(grid_size-1), GL_UNSIGNED_INT, 0);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    cam.updateDirection(xpos, ypos, firstMouse);
}