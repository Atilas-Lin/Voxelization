#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"
using namespace std;

class Model
{
public:
	/* Model data */
	vector<Texture> textures_loaded; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory;

	/* Functions */
	Model(string const path);
	void Draw(Shader *shader);
	void getBondingBox(glm::vec3 &min, glm::vec3 &max);

private:
	glm::vec3 m_min, m_max;
	/* Functions */
	void loadModel(string const path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
	
};

unsigned int TextureFromFile(const char *path, const string &directory);
