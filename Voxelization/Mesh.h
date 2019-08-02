#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include"Shader.h"
#include<GL/glew.h>
using namespace glm;

struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	Mesh(float vertices[]);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	~Mesh();
	void setAABB(const vec3 &min, const vec3 &max);
	bool getAABB(vec3 &min, vec3 &max);
	void Draw(Shader* shader);

private:
	unsigned int VAO, VBO, EBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	glm::vec3 m_minVert, m_maxVert;
	void setupMesh();
	
};

