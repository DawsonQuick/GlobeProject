#pragma once
#include <string>
#include <map>
#include <vector>

#include "./StaticTerrain/StaticTerrain.h"
#include "./InstancedTerrain/InstancedTerrain.h"

/*
* Terrain will be broken into two different sections
*	
*	Static geometry, which will be all in one buffer and be renderered at the same time
*		-Note, this type will be used for geometry that is unique (ie. only a single instance)
*			-Reasoning for this, there is no need to add another opengl call for each unique geometry. Just throw it all into one buffer
* 
*	Intanced geometry, which will be for any gemoetry that is repeated within a scene.
*		-There can be multiple instanced of Instanced Geometry.
*		  Which means there will need to be multiple opengl calls for each set up instanced gemoetry
* 
* 
*	Both types of geometry , should used the same shader to simplify the logic and to reduce the draw calls.
*	Unique information will be tranferred in the unique buffers of the geometry
*/
class TerrainManager {
public:
	TerrainManager();
	~TerrainManager();
	void render();

private:
	StaticTerrain staticTerrain;
	std::vector<InstancedTerrain> instancedTerrain;
};