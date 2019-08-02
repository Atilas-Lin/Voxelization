#include "Model.h"


Model::Model(string const path)
{
	loadModel(path);
}

void Model::Draw(Shader * shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void Model::getBondingBox(glm::vec3 &min, glm::vec3 &max)
{
	min = m_min;
	max = m_max;
}

void Model::loadModel(string const path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('\\'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

// Processes a node in a recursive fashion. 
// Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode * node, const aiScene * scene)
{
	//std::cout << node->mName.data << std::endl;

	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* curMesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(curMesh, scene));
	}

	// after processed all of the meshes, then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

// Transform ASSIMP's aiMesh to our data struct 'Mesh'
Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	// data to fill
	vector<Vertex> tempVertices;
	vector<unsigned int> tempIndices;
	vector<Texture> tempTextures;

	glm::vec3 tempVec;
	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex tempVertex;
		// positions
		tempVertex.Position.x = mesh->mVertices[i].x;
		tempVertex.Position.y = mesh->mVertices[i].y;
		tempVertex.Position.z = mesh->mVertices[i].z;
		// normals
		tempVertex.Normal.x = mesh->mNormals[i].x;
		tempVertex.Normal.y = mesh->mNormals[i].y;
		tempVertex.Normal.z = mesh->mNormals[i].z;
		// texture coordinates
		if (mesh->mTextureCoords[0]) {
			tempVertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			tempVertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			tempVertex.TexCoords = glm::vec2(0, 0);
		}
		tempVertices.push_back(tempVertex);

		// bounding box.
		if (mesh->mVertices[i].x < m_min.x)
			m_min.x = mesh->mVertices[i].x;
		if (mesh->mVertices[i].y < m_min.y)
			m_min.y = mesh->mVertices[i].y;
		if (mesh->mVertices[i].z < m_min.z)
			m_min.z = mesh->mVertices[i].z;
		if (mesh->mVertices[i].x > m_max.x)
			m_max.x = mesh->mVertices[i].x;
		if (mesh->mVertices[i].y > m_max.y)
			m_max.y = mesh->mVertices[i].y;
		if (mesh->mVertices[i].z > m_max.z)
			m_max.z = mesh->mVertices[i].z;
	}

	// wak through each of the mesh's faces and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			tempIndices.push_back(mesh->mFaces[i].mIndices[j]);
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	tempTextures.insert(tempTextures.end(), diffuseMaps.begin(), diffuseMaps.end());

	// specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	tempTextures.insert(tempTextures.end(), specularMaps.begin(), specularMaps.end());

	// normal maps
	vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
	tempTextures.insert(tempTextures.end(), normalMaps.begin(), normalMaps.end());

	// height maps
	vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
	tempTextures.insert(tempTextures.end(), heightMaps.begin(), heightMaps.end());

	return Mesh(tempVertices, tempIndices, tempTextures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char * path, const string & directory)
{
	string filename = string(path);
	filename = directory + '\\' + filename;
	std::cout << filename << std::endl;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
