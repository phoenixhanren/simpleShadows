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

struct Texture {
	unsigned int id;
	std::string type;
	aiString path;
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
	void Draw(Shader shader);

	
	unsigned int VAO;

private:

	unsigned int VBO, EBO;

	/*
		初始化缓冲
	*/
	void setupMesh();
};

#endif // !MESH_H