#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLchar *LoadShader(const std::string &file);

class cShader
{
public:
    unsigned int ID;
    int wgWidth;
    int wgHeight;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    cShader(const std::string &computePath, int wid, int hig)
    {
        wgWidth = wid;
        wgHeight = hig;

        const GLchar *cShaderCode = LoadShader(computePath);
        
        unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);

        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        checkCompileErrors(compute, "COMPUTE");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, compute);
        glDeleteShader(compute);
    }

    void link_shader(const std::string &new_shader_path){
        const GLchar *new_code = LoadShader(new_shader_path);

        unsigned int new_shader = glCreateShader(GL_COMPUTE_SHADER);

        glShaderSource(new_shader, 1, &new_code, NULL);
        glCompileShader(new_shader);

        checkCompileErrors(new_shader, "NEW CODE");
        glAttachShader(ID, new_shader);
        glDeleteShader(new_shader);
    }

    void compile_shader(){
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
    }

    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(ID); 
        glDispatchCompute(128, 128, 1);
        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
    int getID(){
        return ID;
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        //std::cout << name << std::endl; 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }

    void setVec2(const std::string &name, float value1, float value2) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), value1, value2);
    }

    void setVec4(const std::string &name, float value1, float value2, float value3, float value4) const
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3, value4);
    }

    void setMat3(const std::string &name, glm::mat3 &value) const
    { 
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); 
    }

    void setMat4(const std::string &name, glm::mat4 &value) const
    { 
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); 
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- ----------------------------------------------------- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- ----------------------------------------------------- " << std::endl;
            }
        }
    }
};