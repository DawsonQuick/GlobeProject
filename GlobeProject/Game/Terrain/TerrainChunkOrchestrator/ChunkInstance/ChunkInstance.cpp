#include "ChunkInstance.h"
#include <GL/glew.h>


ChunkInstance::ChunkInstance() {

}

ChunkInstance::ChunkInstance(ComputeTerrainGenInfo_OUT& mapData, int xOffset, int yOffset)
{
	glGenVertexArrays(1, &m_VOA);

	glGenTextures(1, &m_MapBuffer);
	generateVertices(m_VOA, mapData.width*30, mapData.height*30, 4 , xOffset - 3  , yOffset - 3);
	bindMapTexture(m_MapBuffer, mapData.outData, mapData.width, mapData.height);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
}

ChunkInstance::~ChunkInstance() {
	glDeleteVertexArrays(1, &m_VOA);

	glDeleteTextures(1, &m_MapBuffer);

}

void ChunkInstance::render() {
	glBindTexture(GL_TEXTURE_2D, m_MapBuffer);
	glBindVertexArray(m_VOA);
								//Number of patches * res * res
	glDrawArrays(GL_PATCHES, 0, 4 * 4 * 4);

}

