#include "Mesh.h"

void Mesh::Draw(Shader shader, GLenum drawType)
{
	unsigned int diffuseNum = 1;
	unsigned int specularNum = 1;
	unsigned int reflectNum = 1;

	for (unsigned int i = 0; i < Textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		stringstream ss;
		string number;
		string name = Textures[i].type;

		if (name == "texture_diffuse")
		{
			ss << diffuseNum++;
		}
		else if (name == "texture_specular")
		{
			ss << specularNum++;
		}
		else if (name == "texture_reflect")
		{
			ss << reflectNum++;
		}
		number = ss.str();

		shader.set_float(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, Textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glDrawElements(drawType, Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(GL_FLOAT)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(6 * sizeof(GL_FLOAT)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
}
