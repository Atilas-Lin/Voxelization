#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Voxelization
{
public:
	Voxelization();
	~Voxelization();
	void getAABB(glm::vec3 &min, glm::vec3 &max);
	bool voxelize();
};

