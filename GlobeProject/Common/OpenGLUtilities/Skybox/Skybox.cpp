#include "Skybox.h"
unsigned int Skybox::compileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);

    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return id;
}

unsigned int Skybox::createShaderProgram() {

    unsigned int vertexShader;
    unsigned int fragmentShader;

    vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
   

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

Skybox::Skybox() {
    m_ShaderProgram = createShaderProgram();

    loadedTempData = false;
    loadedBuffer = false;

    m_model = glm::mat4(1.0);
    m_model = glm::scale(m_model, glm::vec3(100000000.0));

    m_cachedUniforms = false;
    unsigned int skyboxVBO;
    glGenVertexArrays(1, &m_SkyBoxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(m_SkyBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, skyboxBaseVertices.size() * sizeof(float), skyboxBaseVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

}

Skybox::~Skybox() {

}

void Skybox::loadBuffer() {
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
    int index = 0;
    for (SkyboxData& data : m_tempData) {
        if (data.data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, GL_RGBA, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data);
            stbi_image_free(data.data);
            data.data = nullptr;
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << std::endl;
            stbi_image_free(data.data);
            data.data = nullptr;
        }
        index++;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    loadedTempData = false;
    loadedBuffer = true;
    m_tempData.clear();
    m_tempData.shrink_to_fit();
    skyBoxLoaderThread.stop();
}

void Skybox::RenderSkyBox(glm::mat4 view, glm::mat4 proj) {
    if (loadedTempData) {
        loadBuffer();
    }

    if (loadedBuffer) {
        glDepthFunc(GL_LEQUAL);
        glUseProgram(m_ShaderProgram);

        if (!m_cachedUniforms) {
            m_Model = glGetUniformLocation(m_ShaderProgram, "model");
            m_View = glGetUniformLocation(m_ShaderProgram, "view");
            m_Projection = glGetUniformLocation(m_ShaderProgram, "projection");
            m_skybox = glGetUniformLocation(m_ShaderProgram, "skybox");
            m_cachedUniforms = true;
        }

        glUniformMatrix4fv(m_Model, 1, GL_FALSE, glm::value_ptr(m_model));
        glUniformMatrix4fv(m_View, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(m_Projection, 1, GL_FALSE, glm::value_ptr(proj));


        // skybox cube
        glBindVertexArray(m_SkyBoxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }

}




void Skybox::loadCubemap(std::vector<std::string> faces)
{
    
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        SkyboxData tempData;
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        tempData.data = data;
        tempData.width = width;
        tempData.height = height;
        m_tempData.push_back(tempData);
    }
    loadedTempData = true;
}



void Skybox::loadSkyBox(const char* skyboxLocation) {

    std::vector<std::string> tmpFaces;

    for (std::string face : faces) {
        std::string tempString = std::string(skyboxLocation) + "/" + face;
        tmpFaces.push_back(tempString);
    }


    skyBoxLoaderThread.start(&Skybox::loadCubemap, this, tmpFaces);
    //stbi_set_flip_vertically_on_load(false);
    //loadCubemap(tmpFaces);

}