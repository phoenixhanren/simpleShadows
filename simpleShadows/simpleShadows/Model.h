#pragma once
#ifndef MODEL_H
#define MODEL_H
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // !GLEW_STATIC

//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#endif
#include <stb_image.h>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Shader.h"
#include "Mesh.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <utility>

using std::vector;
using std::string;
using std::move;
using std::strcmp;
using std::cout;
using std::endl;

unsigned int TextureFromFile(const char * path, const string & directory, bool gamma = false);

class Model
{
public:

	Model(const char * path, bool generateAdjacencies = false) {
		loadModel(path, generateAdjacencies);
	}

	void Draw(Shader shader);
	vector<Mesh> meshes;
private:

	vector<Texture> texturesLoaded;
	string directory;

	void loadModel(string path, bool generateAdjacencies = false);
	void processNode(aiNode * node, const aiScene * scene, bool generateAdjacencies = false);
	Mesh processMesh(aiMesh * mesh, const aiScene * scene, bool generateAdjacencies = false);
	vector<Texture> loadMaterialTextures(aiMaterial * mat,
		aiTextureType type, string typeName);

};
#endif