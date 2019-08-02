#include "Mesh.h"

Mesh::Mesh(float vertices[])
{
	this->vertices.resize(36);
	memcpy(&(this->vertices[0]), vertices, 36 * 8 * sizeof(float));

	setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh(); // initial the model's mesh
}

Mesh::~Mesh()
{
}

void Mesh::setAABB(const vec3 & min, const vec3 & max)
{
	m_minVert = min; m_maxVert = max;
}

bool Mesh::getAABB(vec3 & min, vec3 & max)
{
	min = m_minVert; max = m_maxVert;
}

void Mesh::Draw(Shader * shader)
{                                 //有幾張texture就要編譯幾次    
	for (unsigned int  i = 0; i < textures.size(); i++)
	{
		if (textures[i].type == "texture_diffuse")
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
			shader->setInt("material.diffuse", Shader::DIFFUSE);
		}
		else if (textures[i].type == "texture_specular")
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
			shader->setInt("material.specular", Shader::SPECULAR);
		}
	}

	// Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	// Unbind - set everything back to defaults once configured.
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)) );
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6*sizeof(float)) );

	glBindVertexArray(0);
}
