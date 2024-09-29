#include "ChunkInstance.h"
#include <GL/glew.h>


ChunkInstance::ChunkInstance() {

}

ChunkInstance::ChunkInstance(ComputeTerrainGenInfo_OUT& mapData, int xOffset, int yOffset)
{
	glGenVertexArrays(1, &m_VOA);

	//glGenTextures(1, &m_MapBuffer);
	glGenTextures(1, &m_HeightBuffer);
	glGenTextures(1, &m_NormalBuffer);
	glGenTextures(1, &m_GradientBuffer);

	generateVertices(m_VOA, mapData.width*300, mapData.height*300, 4 , xOffset - 2 , yOffset - 2);
	//bindMapTexture(m_MapBuffer, mapData.outData, mapData.width, mapData.height);
	bindHeightTexture(m_HeightBuffer, mapData.heightData, mapData.width, mapData.height);
	bindRGBTexture(m_NormalBuffer, mapData.normalData, mapData.width, mapData.height);
	bindRGBTexture(m_GradientBuffer, mapData.gradientData, mapData.width, mapData.height);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
}

ChunkInstance::~ChunkInstance() {
	glDeleteVertexArrays(1, &m_VOA);

	//glDeleteTextures(1, &m_MapBuffer);
	glDeleteTextures(1, &m_HeightBuffer);
	glDeleteTextures(1, &m_NormalBuffer);
	glDeleteTextures(1, &m_GradientBuffer);

}

void ChunkInstance::render() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_HeightBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_NormalBuffer);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_GradientBuffer);
	glBindVertexArray(m_VOA);
								//Number of patches * res * res
	glDrawArrays(GL_PATCHES, 0, 4 * 4 * 4);

}

