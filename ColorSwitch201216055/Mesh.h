#pragma once
#include <string>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\geometric.hpp>
class Mesh
{
public:
	Mesh();

	enum Color {
		RED,
		GREEN,
		BLUE,
		WHITE
	};

	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);
	Color GetCheckColor();
	glm::vec3 GetObjColor();
	void SetColor(Color color);
	void RenderMesh();
	void ClearMesh();

	~Mesh();

private: 
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
	Color checkColor;
	glm::vec3 objColor;
};

