#include "Model.h"

unsigned int TextureFromFile(const char * path, const string & directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

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
		std::cout << "Texture failed to load at path: " << filename.c_str() << std::endl;
		stbi_image_free(data);
	}

	return textureID;


	return 0;
}


void Model::Draw(Shader shader)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		if (generateAdjacencies)
		{
			meshes[i].Draw(shader, GL_TRIANGLES_ADJACENCY);
		}
		else
		{
			meshes[i].Draw(shader);
		}
		
	}
}

void Model::loadModel(string path)
{
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path,
		aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		// pos norm tex
		vertex.Position = glm::vec3(mesh->mVertices[i].x,
			mesh->mVertices[i].y, mesh->mVertices[i].z);
		if (mesh->mNormals != NULL)
		{
			vertex.Normal = glm::vec3(mesh->mNormals[i].x,
				mesh->mNormals[i].y, mesh->mNormals[i].z);
		}
		else
		{

		}
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// indices
	if (generateAdjacencies)
	{
		findAdjacencies(mesh, indices);
	}
	else
	{
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
	}


	// materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		vector<Texture> specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		vector<Texture> reflectMaps = loadMaterialTextures(material,
			aiTextureType_AMBIENT, "texture_reflect");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		textures.insert(textures.end(), reflectMaps.begin(), reflectMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool exist = false;

		for (unsigned int j = 0; j != texturesLoaded.size(); ++j)
		{
			if (strcmp(texturesLoaded[j].path.C_Str(), str.C_Str()) == 0)
			{
				exist = true;
				textures.push_back(texturesLoaded[j]);
				break;
			}
		}
		if (!exist)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			texturesLoaded.push_back(texture);
		}

	}

	return move(textures);
}

void Model::findAdjacencies(const aiMesh * mesh, vector<unsigned int>& ind)
{
	vector<Face> uniqueFace;
	map<aiVector3D, unsigned int> posMap;
	map<Edge, vector<unsigned int>> neighborsMap;

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace & face = mesh->mFaces[i];

		Face unique(0, 0, 0);

		for (unsigned int j = 0; j < 3; ++j)
		{
			unsigned int index = face.mIndices[j];
			aiVector3D & v = mesh->mVertices[index];

			if (posMap.find(v) == posMap.end())
			{
				posMap[v] = index;
			}
			else
			{
				index = posMap[v];
			}

			unique.indices[j] = index;
		}

		uniqueFace.push_back(unique);

		Edge e1(unique.indices[0], unique.indices[1]);
		Edge e2(unique.indices[1], unique.indices[2]);
		Edge e3(unique.indices[2], unique.indices[0]);

		neighborsMap[e1].push_back(i);
		neighborsMap[e2].push_back(i);
		neighborsMap[e3].push_back(i);
	}
	//vector<GLuint> testInd;
	//for (auto &i : uniqueFace)
	//{
	//	testInd.push_back(i.indices[0]);
	//	testInd.push_back(i.indices[1]);
	//	testInd.push_back(i.indices[2]);
	//}
	//determineAdjacency(testInd);
	//vector<unsigned int> ind1 = testInd;
	//cout << "mesh->mNumFaces..." << mesh->mNumFaces << endl;
	//cout << "uniqueFaces.size()..." << uniqueFace.size() << endl;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		const Face & face = uniqueFace[i];

		for (unsigned int j = 0; j < 3; ++j)
		{
			Edge e(face.indices[j], face.indices[(j + 1) % 3]);
			vector<unsigned int> neighbors = neighborsMap[e];
			int otherFaceInd = neighbors[0];
			if (otherFaceInd == i)
			{
				otherFaceInd = neighbors[1];
			}

			const Face & otherFace = uniqueFace[otherFaceInd];

			unsigned int oppoIndex;
			oppoIndex = otherFace.GetOppositeIndex(e);
			//bool hasSame = false;
			//for (int i = 0; i != 3; ++i)
			//{
			//	oppoIndex = otherFace.indices[i];
			//	if (oppoIndex != e.indices[0] && oppoIndex != e.indices[1])
			//	{
			//		break;
			//	}
			//}
			//for (unsigned int fInd : otherFace.indices)
			//{
			//	for (unsigned int eInd : e.indices)
			//	{
			//		if (fInd == eInd)
			//		{
			//			hasSame = true;
			//			break;
			//		}
			//	}
			//	if (!hasSame)
			//	{
			//		oppoIndex = fInd;
			//		break;
			//	}
			//	hasSame = false;
			//}
			ind.push_back(face.indices[j]);
			ind.push_back(oppoIndex);
		}

	}
#pragma region test
	//int count = 0;
	////for (auto i : ind)
	////{
	////	++count;
	////	if (count % 6 != 0)
	////	{
	////		cout << i << " ";
	////	}
	////	else
	////	{
	////		cout << endl;
	////	}
	////}
	////cout << "=======================" << endl;

	////count = 0;
	////for (auto i : testInd)
	////{
	////	++count;
	////	if (count % 6 != 0)
	////	{
	////		cout << i << " ";
	////	}
	////	else
	////	{
	////		cout << endl;
	////	}
	////}
	////cout << "=======================" << endl;
	////cout << "23333" << endl; 

	//count = 0;

	//for (int i = 0; i < ind.size() && i < testInd.size(); ++i)
	//{
	//	if (ind[i] != testInd[i])
	//	{
	//		++count;
	//		cout << ind[i] << " ... " << testInd[i] << endl;
	//		for (int j = 0; j != i + 1; ++j)
	//		{
	//			cout << ind[j] << " ";
	//		}
	//		cout << endl;
	//		for (int j = 0; j != i + 1; ++j)
	//		{
	//			cout << testInd[j] << " ";
	//		}
	//		cout << endl;
	//		cout << endl;

	//	}
	//}
	//cout << "total difference " << count << endl;
#pragma endregion


}

void Model::determineAdjacency(vector<GLuint>& el)
{

	// Elements with adjacency info
	vector<GLuint> elAdj;

	// Copy and make room for adjacency info
	for (GLuint i = 0; i < el.size(); i += 3)
	{
		elAdj.push_back(el[i]);
		elAdj.push_back(-1);
		elAdj.push_back(el[i + 1]);
		elAdj.push_back(-1);
		elAdj.push_back(el[i + 2]);
		elAdj.push_back(-1);
	}

	// Find matching edges
	for (GLuint i = 0; i < elAdj.size(); i += 6)
	{
		// A triangle
		int a1 = elAdj[i];
		int b1 = elAdj[i + 2];
		int c1 = elAdj[i + 4];

		// Scan subsequent triangles
		for (GLuint j = i + 6; j < elAdj.size(); j += 6)
		{
			int a2 = elAdj[j];
			int b2 = elAdj[j + 2];
			int c2 = elAdj[j + 4];

			// Edge 1 == Edge 1
			if ((a1 == a2 && b1 == b2) || (a1 == b2 && b1 == a2))
			{
				elAdj[i + 1] = c2;
				elAdj[j + 1] = c1;
			}
			// Edge 1 == Edge 2
			if ((a1 == b2 && b1 == c2) || (a1 == c2 && b1 == b2))
			{
				elAdj[i + 1] = a2;
				elAdj[j + 3] = c1;
			}
			// Edge 1 == Edge 3
			if ((a1 == c2 && b1 == a2) || (a1 == a2 && b1 == c2))
			{
				elAdj[i + 1] = b2;
				elAdj[j + 5] = c1;
			}
			// Edge 2 == Edge 1
			if ((b1 == a2 && c1 == b2) || (b1 == b2 && c1 == a2))
			{
				elAdj[i + 3] = c2;
				elAdj[j + 1] = a1;
			}
			// Edge 2 == Edge 2
			if ((b1 == b2 && c1 == c2) || (b1 == c2 && c1 == b2))
			{
				elAdj[i + 3] = a2;
				elAdj[j + 3] = a1;
			}
			// Edge 2 == Edge 3
			if ((b1 == c2 && c1 == a2) || (b1 == a2 && c1 == c2))
			{
				elAdj[i + 3] = b2;
				elAdj[j + 5] = a1;
			}
			// Edge 3 == Edge 1
			if ((c1 == a2 && a1 == b2) || (c1 == b2 && a1 == a2))
			{
				elAdj[i + 5] = c2;
				elAdj[j + 1] = b1;
			}
			// Edge 3 == Edge 2
			if ((c1 == b2 && a1 == c2) || (c1 == c2 && a1 == b2))
			{
				elAdj[i + 5] = a2;
				elAdj[j + 3] = b1;
			}
			// Edge 3 == Edge 3
			if ((c1 == c2 && a1 == a2) || (c1 == a2 && a1 == c2))
			{
				elAdj[i + 5] = b2;
				elAdj[j + 5] = b1;
			}
		}
	}

	// Look for any outside edges
	for (GLuint i = 0; i < elAdj.size(); i += 6)
	{
		if (elAdj[i + 1] == -1) elAdj[i + 1] = elAdj[i + 4];
		if (elAdj[i + 3] == -1) elAdj[i + 3] = elAdj[i];
		if (elAdj[i + 5] == -1) elAdj[i + 5] = elAdj[i + 2];
	}

	// Copy all data back into el
	el = elAdj;
}


