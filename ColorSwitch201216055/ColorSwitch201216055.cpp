// ColorSwitch201216055.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <GL\glew.h>
#include <cmath>
#include <vector> 

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <gl\GLU.h>
#include <stdlib.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

bool init();
bool initGL();
void render();
void close();
void CreateObjects();
void CreateShaders();

float width = 640, height = 480;
float wallsPositions[15] = {-5.0, -15.0, -20.0, -25.0, -30.0, -35.0, -40.0, -45.0, -50.0, -55.0, -60.0 };


std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

float pyrPosition = 0.0f;
float pyrMove = 0.08f;
float offset = 0.02;
int wallNumber = 0;
Mesh *currWall;
bool gameOver = false;
Mesh::Color pyramidColor = Mesh::GREEN;

//Vertex Shader
static const char* vShader = "Shaders/shader.vert";

//Fragment Shader
static const char* fShader = "Shaders/shader.frag";

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;

void HandleKeyUp(const SDL_KeyboardEvent& key);

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 100.0f);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//event handlers
void HandleKeyDown(const SDL_KeyboardEvent& key);

int main(int argc, char* args[])
{
	init();

	SDL_Event e;
	//While application is running
	bool quit = false;

	CreateObjects();
	CreateShaders();


	while (!quit)
	{

		float currentFrame = SDL_GetTicks() / 1000.0f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (!gameOver) {
			pyrPosition -= pyrMove;
			camera.MoveCamera(pyrPosition + 8);
		}
		

		if (pyrPosition <= wallsPositions[wallNumber] + 0.1)
		{
			if (currWall != NULL)
			{
				if (meshList[1]->GetCheckColor() == currWall->GetCheckColor())
				{
					wallNumber++;
					pyrMove += 0.01;

				}
				else {
					gameOver = true;
				}
			}
			
		}

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				else {
					HandleKeyUp(e.key);
				}
				break;
			}
		}

		//Render quad
		render();

		//Update screen
		SDL_GL_SwapWindow(gWindow);
	}

	close();

	return 0;
}

void HandleKeyUp(const SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym)
	{
	case SDLK_w:
		camera.ProcessKeyboard(camera.FORWARD, deltaTime);
		break;
	case SDLK_s:
		camera.ProcessKeyboard(camera.BACKWARD, deltaTime);
		break;
	case SDLK_a:
		camera.ProcessKeyboard(camera.LEFT, deltaTime);
		break;
	case SDLK_d:
		camera.ProcessKeyboard(camera.RIGHT, deltaTime);
		break;
	case SDLK_q:
		camera.ProcessKeyboard(camera.UP, deltaTime);
		break;
	case SDLK_e:
		camera.ProcessKeyboard(camera.DOWN, deltaTime);
		break;
	case SDLK_KP_1:
		pyramidColor = Mesh::RED;
		break;
	case SDLK_KP_2:
		pyramidColor = Mesh::GREEN;
		break;
	case SDLK_KP_3:
		pyramidColor = Mesh::BLUE;
		break;
	}
}

void HandleKeyDown(const SDL_KeyboardEvent& key)
{
	
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Use OpenGL 3.3
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);


		//Create window
		gWindow = SDL_CreateWindow("Color Switch", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN*/);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Use Vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				//Initialize OpenGL
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}

	return success;
}

bool initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;

	glewInit();

	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		success = false;
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	return success;
}

void close()
{
	

	//Delete OGL context
	SDL_GL_DeleteContext(gContext);
	//Destroy window	
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void render() 
{

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint objColorLocation = 0;

	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	

	shaderList[0].UseShader();
	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	objColorLocation = glGetUniformLocation(shaderList[0].GetShaderID(), "objColor");

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glm::mat4 floor = glm::mat4(1.0);
	
	// model = glm::rotate(model, glm::radians(curAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	// model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
	floor = glm::translate(floor, glm::vec3(0.0f, -2.0f, -28.0f));
	floor = glm::scale(floor, glm::vec3(5.0f, 0.2f, 40.0f));
	
	
	

	//glUniform1f(uniformModel, triOffset);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(floor));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[0]->GetObjColor()));
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

	if (!meshList.empty())
	{
		meshList[0]->RenderMesh();
	}
	//--------------------------------------------------------
	glm::mat4 pyr = glm::mat4(1.0);

	pyr = glm::scale(pyr, glm::vec3(0.4f, 0.4f, 0.4f));
	pyr = glm::translate(pyr, glm::vec3(0.0f, -3.3f, pyrPosition * 2.5));

	// pyr = glm::rotate(pyr, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	meshList[1]->SetColor(pyramidColor);


	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(pyr));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[1]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[1]->RenderMesh();
	}
	
	// -------------------------------------------------------
	glm::mat4 wallRed = glm::mat4(1.0);
	
	wallRed = glm::translate(wallRed, glm::vec3(-0.0f, -1.0f, -5.0f));
	wallRed = glm::scale(wallRed, glm::vec3(4.0f, 0.6f, 0.01f));
	
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallRed));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[2]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[2]->RenderMesh();
	}
	if (wallNumber == 0) currWall = meshList[2];
	//----------------------------------------------------------
	
	glm::mat4 wallGreen = glm::mat4(1.0);

	wallGreen = glm::translate(wallGreen, glm::vec3(-0.0f, -1.0f, -15.0f));
	wallGreen = glm::scale(wallGreen, glm::vec3(4.0f, 0.6f, 0.01f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallGreen));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[3]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[3]->RenderMesh();
	}
	if (wallNumber == 1) currWall = meshList[3];
	//------------------------------------------------------------
	glm::mat4 wallBlue = glm::mat4(1.0);

	wallBlue = glm::translate(wallBlue, glm::vec3(-0.0f, -1.0f, -20.0f));
	wallBlue = glm::scale(wallBlue, glm::vec3(4.0f, 0.6f, 0.01f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallBlue));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[4]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[4]->RenderMesh();
	}
	if (wallNumber == 2) currWall = meshList[4];
	// --------------------------------------------------------------
	wallRed = glm::mat4(1.0);
	wallRed = glm::translate(wallRed, glm::vec3(-0.0f, -1.0f, -25.0f));
	wallRed = glm::scale(wallRed, glm::vec3(4.0f, 0.6f, 0.01f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallRed));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[2]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[2]->RenderMesh();
	}
	if (wallNumber == 3) currWall = meshList[2];
	// --------------------------------------------------------------------
	wallBlue = glm::mat4(1.0);

	wallBlue = glm::translate(wallBlue, glm::vec3(-0.0f, -1.0f, -30.0f));
	wallBlue = glm::scale(wallBlue, glm::vec3(4.0f, 0.6f, 0.01f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallBlue));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[4]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[4]->RenderMesh();
	}
	if (wallNumber == 4) currWall = meshList[4];
	// --------------------------------------------------------------
	wallRed = glm::mat4(1.0);
	wallRed = glm::translate(wallRed, glm::vec3(-0.0f, -1.0f, -35.0f));
	wallRed = glm::scale(wallRed, glm::vec3(4.0f, 0.6f, 0.01f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallRed));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[2]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[2]->RenderMesh();
	}
	if (wallNumber == 5) currWall = meshList[2];
	//----------------------------------------------------------

	wallGreen = glm::mat4(1.0);

	wallGreen = glm::translate(wallGreen, glm::vec3(-0.0f, -1.0f, -40.0f));
	wallGreen = glm::scale(wallGreen, glm::vec3(4.0f, 0.6f, 0.01f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallGreen));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[3]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[3]->RenderMesh();
	}
	if (wallNumber == 6) currWall = meshList[3];
	// --------------------------------------------------------------------
	wallBlue = glm::mat4(1.0);

	wallBlue = glm::translate(wallBlue, glm::vec3(-0.0f, -1.0f, -45.0f));
	wallBlue = glm::scale(wallBlue, glm::vec3(4.0f, 0.6f, 0.01f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallBlue));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[4]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[4]->RenderMesh();
	}
	if (wallNumber == 7) currWall = meshList[4];
	//--------------------------------------------------------
	wallRed = glm::mat4(1.0);

	wallRed = glm::translate(wallRed, glm::vec3(-0.0f, -1.0f, -50.0f));
	wallRed = glm::scale(wallRed, glm::vec3(4.0f, 0.6f, 0.01f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallRed));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[2]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[2]->RenderMesh();
	}
	if (wallNumber == 8) currWall = meshList[2];
	//----------------------------------------------------------

	wallGreen = glm::mat4(1.0);

	wallGreen = glm::translate(wallGreen, glm::vec3(-0.0f, -1.0f, -55.0f));
	wallGreen = glm::scale(wallGreen, glm::vec3(4.0f, 0.6f, 0.01f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallGreen));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[3]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[3]->RenderMesh();
	}
	if (wallNumber == 9) currWall = meshList[3];
	//------------------------------------------------------------
	wallBlue = glm::mat4(1.0);

	wallBlue = glm::translate(wallBlue, glm::vec3(-0.0f, -1.0f, -60.0f));
	wallBlue = glm::scale(wallBlue, glm::vec3(4.0f, 0.6f, 0.01f));

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wallBlue));
	glUniform3fv(objColorLocation, 1, glm::value_ptr(meshList[4]->GetObjColor()));
	if (!meshList.empty())
	{
		meshList[4]->RenderMesh();
	}
	if (wallNumber == 10) currWall = meshList[4];


	glUseProgram(0);

}

void CreateObjects()
{
	unsigned int indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat vertices[] = {
		
		// front
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0

	}; 

	GLfloat verticesForPyr[] = {
		-1.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f

	};

	unsigned int indicesForPyr[] = {
		0, 3, 1, 
		1, 3, 2,
		2, 3, 0, 
		0, 1, 2

	};

	Mesh *floor = new Mesh();
	floor->CreateMesh(vertices, indices, 36, 12*3);
	floor->SetColor(floor->WHITE);
	meshList.push_back(floor);

	Mesh *pyramid = new Mesh();
	pyramid->CreateMesh(verticesForPyr, indicesForPyr, 12, 12);
	pyramid->SetColor(pyramid->GREEN);
	meshList.push_back(pyramid);

	Mesh *redWall = new Mesh();
	redWall->CreateMesh(vertices, indices, 36, 12 * 3);
	redWall->SetColor(redWall->RED);
	meshList.push_back(redWall);

	Mesh *greenWall = new Mesh();
	greenWall->CreateMesh(vertices, indices, 36, 12 * 3);
	greenWall->SetColor(greenWall->GREEN);
	meshList.push_back(greenWall);

	Mesh *blueWall = new Mesh();
	blueWall->CreateMesh(vertices, indices, 36, 12 * 3);
	blueWall->SetColor(blueWall->BLUE);
	meshList.push_back(blueWall);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


