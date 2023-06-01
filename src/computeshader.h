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
        //const GLchar* funcCode = LoadShader("shaders(comp)/mathfunc.comp");
        
        unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
        //unsigned int function = glCreateShader(GL_COMPUTE_SHADER);

        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);

        //glShaderSource(function, 1, &funcCode, NULL);
        //glCompileShader(function);
        checkCompileErrors(compute, "COMPUTE");
        //checkCompileErrors(function, "FUNCTION");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, compute);
        //glAttachShader(ID, function);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(compute);
        //glDeleteShader(function);
    }

    //second constructor for line insertion
    /*cShader(std::string computePath, int wid, int hig, int insline, std::string configPath, std::string metricPath)
    {
        wgWidth = wid;
        wgHeight = hig;
        // 1. retrieve the vertex/fragment source code from filePath
        std::string computeCode;
        std::fstream cShaderFile;

        //std::string configPath = "WormHoleTest/config.glsl";
        //std::string metricPath = "WormHoleTest/metric.glsl";

        //retrieves metric config
        //std::string metricConfig;
        std::fstream metricFile;
        std::fstream configFile;
        int vShaderLines = 1;
        int configLines = 1;
        int metricLines = 1;

        //open file tester
        cShaderFile.open(computePath, std::ios::in);
        std::string cline;
        while(getline(cShaderFile, cline)){
            vShaderLines ++;
        }
        cShaderFile.close(); 

        //open config file
        configFile.open(configPath, std::ios::in);
        std::string dline;
        while(getline(configFile, dline)){
            configLines ++;
        }
        configFile.close();

        metricFile.open(metricPath, std::ios::in);
        std::string mline;
        while(getline(metricFile, mline)){
            metricLines ++;
        }
        metricFile.close();

        int totalLines = vShaderLines;
        
        //read files
        cShaderFile.open(computePath, std::ios::in);
        std::string cline2;
        std::string dline2;
        std::string mline2;
        while(getline(cShaderFile, cline2)){
            vShaderLines --;
            if(totalLines - vShaderLines == insline){
                metricFile.open(metricPath, std::ios::in);
                while(getline(metricFile, mline2)){
                    //std::cout << configLines << std::endl;
                    metricLines --;
                    if(metricLines >= 1){
                        computeCode += mline2 + "\n";
                    }
                }
                metricFile.close();

                configFile.open(configPath, std::ios::in);
                while(getline(configFile, dline2)){
                    //std::cout << configLines << std::endl;
                    configLines --;
                    if(configLines >= 1){
                        computeCode += dline2 + "\n";
                    }
                }
                configFile.close();
            }
            if(vShaderLines > 1){
                computeCode += cline2 + "\n";
            }
            else{
                computeCode += cline2 + "\0";    
            }
        }
        cShaderFile.close();
        //std::cout << computeCode << std::endl;

        const char* cShaderCode = computeCode.c_str();
        
        unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);

        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        checkCompileErrors(compute, "COMPUTE");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, compute);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(compute);
    }*/
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