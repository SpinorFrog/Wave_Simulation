#include <glad/glad.h>
#include "shader.h"
#include <iostream>
#include <string>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture
{
    private:
    Shader *shader;
    unsigned int textures[100];

    public:
    int textureCounter = 0;
    //unsigned int textures;

    Texture(Shader &sref){
        shader = &sref;
    }

    void addTexture(const char* texName, unsigned char *data, int type){
        int width, height, nrChannels;
        glGenTextures(1, &textures[textureCounter]);
        glActiveTexture(GL_TEXTURE3 + textureCounter);
        glBindTexture(GL_TEXTURE_2D, textures[textureCounter]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_set_flip_vertically_on_load(true); //flips image for correction
        data = stbi_load(texName, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB + type, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        (*shader).use();
        int tex = glGetUniformLocation((*shader).getID(), "textures[0]") + textureCounter;
        glUniform1i(tex, textureCounter);

        textureCounter ++;
    }

    void activateTexture(int num){
        glActiveTexture(GL_TEXTURE3 + num);
        glBindTexture(GL_TEXTURE_2D, textures[num]);
    }

    void activateTexture(int num, unsigned int &unum){
        glActiveTexture(GL_TEXTURE0 + num - 1);
        glBindTexture(GL_TEXTURE_2D, unum);
    }

};