#pragma once

#ifndef MESH_H
#define MESH_H

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // !GLEW_STATIC

#include <stb_image.h>
#include <GL\glew.h>
#include <assimp\Importer.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sstream>
#include <string.h>
#include <vector>


#include "Shader.h"

using std::string;
using std::vector;
using std::stringstream;


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
	aiString path;
};

struct Edge
{
	Edge(unsigned int s, unsigned int e) {
		indices[0] = s;
		indices[1] = e;
		if (s > e)
		{
			indices[1] = s;
			indices[0] = e;
		}

	}
	unsigned int indices[2];

	bool operator< (const Edge & right) const {
		if (indices[0] < right.indices[0])
		{
			return true;
		}

		if (indices[0] == right.indices[0])
		{
			return indices[1] < right.indices[1];
		}
		return false;
	}

	bool operator> (const Edge & right) const {
		return !(this->operator<(right));
	}

	bool operator==(const Edge & right) const {
		return indices[0] == right.indices[0] && indices[1] == right.indices[1];
	}

	bool operator!= (const Edge &right) const {
		return *this == right;
	}

};

struct Face
{
	unsigned int indices[3];

	Face(GLuint x, GLuint y, GLuint z) {
		indices[0] = x;
		indices[1] = y;
		indices[2] = z;
	}

	GLuint GetOppositeIndex(const Edge& e) const
	{
		for (GLuint i = 0; i < 3; i++) {
			GLuint Index = indices[i];

			if (Index != e.indices[0] && Index != e.indices[1]) {
				return Index;
			}
		}

		assert(0);

		return 0;
	}



};

class Mesh {
public:

	vector<Vertex> Vertices;
	vector<unsigned int> Indices;
	vector<Texture> Textures;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices,
		vector<Texture> textures) : Vertices(vertices), Indices(indices),
		Textures(textures) {
		setupMesh();
	}

	/*
		绘制网格
	*/
	void Draw(Shader shader, GLenum drawType = GL_TRIANGLES);

	
	unsigned int VAO;

private:

	unsigned int VBO, EBO;

	/*
		初始化缓冲
	*/
	void setupMesh();
};

#endif // !MESH_H