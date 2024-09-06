#include "ChunkInstance.h"
#include <GL/glew.h>


ChunkInstance::ChunkInstance() {

}

ChunkInstance::ChunkInstance(ComputeTerrainGenInfo_OUT& mapData, int xOffset, int yOffset)
{
	glGenVertexArrays(1, &m_VOA);

	glGenTextures(1, &m_MapBuffer);
	glGenTextures(1, &m_HeightBuffer);

	generateVertices(m_VOA, mapData.width*300, mapData.height*300, 4 , xOffset - 8 , yOffset - 8);
	bindMapTexture(m_MapBuffer, mapData.outData, mapData.width, mapData.height);
	//bindHeightTexture(m_HeightBuffer, mapData.heightData, mapData.width, mapData.height);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
}

ChunkInstance::~ChunkInstance() {
	glDeleteVertexArrays(1, &m_VOA);

	glDeleteTextures(1, &m_MapBuffer);
	glDeleteTextures(1, &m_HeightBuffer);

}

void ChunkInstance::render() {
	glBindTexture(GL_TEXTURE_2D, m_MapBuffer);
	glBindVertexArray(m_VOA);
								//Number of patches * res * res
	glDrawArrays(GL_PATCHES, 0, 4 * 4 * 4);

}

