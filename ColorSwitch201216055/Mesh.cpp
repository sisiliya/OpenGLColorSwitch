#include "stdafx.h"
#include "Mesh.h"


Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount = numOfIndices;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

glm::vec3 Mesh::GetObjColor()
{
	return objColor;
}

Mesh::Color Mesh::GetCheckColor()
{
	return checkColor;
}

void Mesh::SetColor(Color color)
{
	switch (color)
	{
	case Mesh::RED:
		objColor = glm::vec3(1.0f, 0.0f, 0.0f);
		checkColor = RED;
		break;
	case Mesh::GREEN:
		objColor = glm::vec3(0.0f, 1.0f, 0.0f);
		checkColor = GREEN;
		break;
	case Mesh::BLUE:
		objColor = glm::vec3(0.0f, 0.0f, 1.0f);
		checkColor = BLUE;
		break;
	case Mesh::WHITE:
		objColor = glm::vec3(1.0f, 1.0f, 1.0f);
		checkColor = WHITE;
		break;
	default:
		break;
	}
}

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
	if (IBO != 0)
	{
		// Deletes IBO off graphics card -> more space
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0)
	{
		// Deletes VBO off graphics card -> more space
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		// Deletes VAO off graphics card -> more space
		glDeleteBuffers(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}

Mesh::~Mesh()
{
	ClearMesh();
}
