#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "./../../Vendor/stb_image/stb_image.h"
#include "./../../Vendor/glm/glm.hpp"
#include "./../../Vendor/glm/gtc/matrix_transform.hpp"
#include "./../../Vendor/glm/gtc/type_ptr.hpp"
#include "./../../Utilities/ThreadWrapper/ThreadWrapper.h"

struct SkyboxData {
    unsigned char* data;
    int width, height;
};

class Skybox {

public:
	Skybox();
	~Skybox();

	void RenderSkyBox(glm::mat4 m_Veiw, glm::mat4 m_Proj);
    void loadSkyBox(const char* skyboxLocation);


private:

    ThreadWrapper skyBoxLoaderThread;

    void loadBuffer();

	glm::mat4 m_model;
	unsigned int m_ShaderProgram;
    unsigned int m_SkyBoxVAO;
    unsigned int m_textureID;

    bool loadedBuffer;
    bool loadedTempData;
    std::vector<SkyboxData> m_tempData;

    bool m_cachedUniforms;
    unsigned int m_Model, m_View, m_Projection, m_skybox;

    

	unsigned int createShaderProgram();
	unsigned int compileShader(unsigned int type, const char* source);
    void loadCubemap(std::vector<std::string> faces);
	const char* vertexShaderSource = R"(
	#version 330 core
	layout (location = 0) in vec3 aPos;

	out vec3 TexCoords;

	uniform mat4 projection;
	uniform mat4 view;
    uniform mat4 model;

	void main()
	{
		TexCoords = aPos;
		vec4 pos = projection * view * model * vec4(aPos.x,aPos.y,aPos.z, 1.0);
		gl_Position = pos.xyww;
	} 
)";
	const char* fragmentShaderSource = R"( 
	#version 330 core
	out vec4 FragColor;

	in vec3 TexCoords;

	uniform samplerCube skybox;

	void main()
	{    
		FragColor = texture(skybox, TexCoords).rgba;
	}
)";

    std::vector<std::string> faces
    {
        "right.png",
        "left.png",
        "top.png",
        "bottom.png",
        "front.png",
        "back.png"
    };

    std::vector<float> skyboxBaseVertices = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

};

#endif