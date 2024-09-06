#pragma once
#ifndef CHUNKINSTANCE_H
#define CHUNKINSTANCE_H

#include "./../Utils/ChunkUtils.h"
#include "./../../../../Common/Logger/Logger.h"
#include "./../../../../Common/TerrainGeneration/PlanarTerrain/ComputeShaderMethod/ComputeShaderPlanarTerrainGeneration.h"

class ChunkInstance {
public:
	ChunkInstance();
	ChunkInstance(ComputeTerrainGenInfo_OUT& mapData, int xOffset, int yOffset);
	~ChunkInstance();

	void render();
private:
	unsigned int m_VOA;
	unsigned int m_MapBuffer;
	unsigned int m_HeightBuffer;
	

};

#endif