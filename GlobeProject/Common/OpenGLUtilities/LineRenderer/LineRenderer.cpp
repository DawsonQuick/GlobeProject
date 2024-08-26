#include "LineRenderer.h"
#include <GL/glew.h>
unsigned int LineRenderer::compileShader(unsigned int type, const char* source) {
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

unsigned int LineRenderer::createShaderProgram() {

    unsigned int vertexShader;
    unsigned int fragmentShader;

    if (isColorsEmbdedded) {
        vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSourceWithColorsSSBO);
        fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSourceWithColorsSSBO);
    }
    else {
        vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSourceSSBO);
        fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSourceSSBO);
    }

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

void LineRenderer::bindBuffers() {
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
    }
    if (VBO == 0) {
        glGenBuffers(1, &VBO);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, trajectoryData.size() * sizeof(float), trajectoryData.data(), GL_STATIC_DRAW);


    if (isColorsEmbdedded) {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    else {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}



LineRenderer::LineRenderer(bool isColorsEmbdedded): isColorsEmbdedded(isColorsEmbdedded) {
    shaderProgram = createShaderProgram();
    m_currentColor = glm::vec3(1.0f, 1.0f, 1.0f);
    m_cachedUniforms = false;

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

LineRenderer::~LineRenderer() {

}

void LineRenderer::appendLines(std::vector<float>& newLines, bool forceRefresh) {
    //trajectoryData.assign(newLines.begin(), newLines.end());
    //bindBuffers();

    updateSSBO(newLines, forceRefresh);
}
void LineRenderer::setColor(glm::vec3 newColor) {
    m_currentColor = newColor;
}


void LineRenderer::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
    
    // Use shader program and set uniforms
    glUseProgram(shaderProgram);

    if (!m_cachedUniforms) {
        m_Model = glGetUniformLocation(shaderProgram, "model");
        m_View = glGetUniformLocation(shaderProgram, "view");
        m_Projection = glGetUniformLocation(shaderProgram, "projection");
        if (!isColorsEmbdedded) {
            m_Colors = glGetUniformLocation(shaderProgram, "color");
        }

        m_cachedUniforms = true;
    }
    glUniformMatrix4fv(m_Model, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(m_View, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_Projection, 1, GL_FALSE, glm::value_ptr(projection));
    if (!isColorsEmbdedded) {
        glUniform3f(m_Colors, m_currentColor.x, m_currentColor.y, m_currentColor.z);
    }

    // Bind VAO and draw the trajectory
    //glBindVertexArray(VAO);
    //glDrawArrays(GL_LINES, 0, trajectoryData.size() / 3);
    //glBindVertexArray(0);

        // Bind the SSBO (already bound via glBindBufferBase, but ensure it's correct)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    // Draw the lines using the SSBO data
    glDrawArrays(GL_LINES, 0, currentBufferSize / 3);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}



//------------PRIVATE METHODS----------------------
void LineRenderer::updateSSBO(const std::vector<float>& newLines, bool forceUpdate) {
    currentBufferSize = newLines.size();

    // Check if we need to resize the SSBO
    if (forceUpdate || currentBufferSize > maxBufferSize) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, currentBufferSize * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        maxBufferSize = currentBufferSize;
    }

    // Update the SSBO with the new data
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    void* ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    if (ptr) {
        std::memcpy(ptr, newLines.data(), currentBufferSize * sizeof(float));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
}
