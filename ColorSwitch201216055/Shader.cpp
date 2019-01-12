#include "stdafx.h"
#include "Shader.h"


Shader::Shader()
{
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open()) {
		printf("Failed to read %s! File doesnt exist", fileLocation);
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	shaderID = glCreateProgram();

	if (!shaderID) {
		printf("Error creating shader program! \n");
		return;
	}

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar elog[1024] = { 0 };

	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(elog), NULL, elog);
		printf("Error linking program %s \n", elog);
		return;
	}

	glValidateProgram(shaderID);

	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(elog), NULL, elog);
		printf("Error validating program %s \n", elog);
		return;
	}

	uniformModel = glGetUniformLocation(shaderID, "model");
	uniformProjection = glGetUniformLocation(shaderID, "projection");
	uniformView = glGetUniformLocation(shaderID, "view");

}

GLuint Shader::GetShaderID()
{
	return shaderID;
}

GLuint Shader::GetProjectionLocation()
{
	return uniformProjection;
}
GLuint Shader::GetModelLocation()
{
	return uniformModel;
}
GLuint Shader::GetViewLocation()
{
	return uniformView;
}



void Shader::UseShader()
{
	glUseProgram(shaderID);
}

void Shader::ClearShader()
{
	if (shaderID != 0)
	{
		glDeleteProgram(shaderID);
		shaderID = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codelength[1];
	codelength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codelength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar elog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(elog), NULL, elog);
		printf("Error compiling the %d shader %s \n", shaderType, elog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

Shader::~Shader()
{
	ClearShader();
}
