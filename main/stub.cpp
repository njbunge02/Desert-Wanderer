/******************************************************************************|
| CPSC 4050/6050 Computer Garphics Assignment 5, Daljit Singh Dhillon, 2020    |
| Reference:                                                                   |
|                                                                              |
| Some OpenGL setup code here including math_funcs, and gl_utils               |
| are from Angton Gerdelan and "Anton's OpenGL 4 Tutorials."                   |
| http://antongerdelan.net/opengl/                                             |
| Email: anton at antongerdelan dot net                                        |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
|******************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>


#include <math.h>
#include <time.h>

#include "maths_funcs.h"   // Anton's maths functions.
#include "gl_utils.h"      // Anton's opengl functions and small utilities like logs
#include "stb_image.h"     // Sean Barrett's image loader with Anton's load_texture()

#include <vector>

#define _USE_MATH_DEFINES
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

mat4 view_mat;
mat4 proj_mat;
mat4 model_mat;
mat4 model_player;


extern float nx_frames;
extern float ny_frames;
extern float uv_x;
extern float uv_y;
extern float playerZ;
extern float backZ;




bool isW;
bool isA;
bool isS;
bool isD;
float spriteX;
float spriteY;
bool jumpTrigger;


using namespace std;

vector<float> floorVerts;
vector<float> floorTexCoords;
vector<float> platVerts;
vector<float> platTexCoords;

vector<float> backgroundVerts;
vector<float> backgroundTexCoords;


vector<float> playerVerts;
vector<float> playerTexCoords;

GLuint playerVao;
GLuint floorVao;
GLuint platVao;
GLuint backVao;


void generateFloor(vector<float> &vertices, vector<float> &texCoords);
void generatePlatform(vector<float> &vertices, vector<float> &texCoords);
void generatePlayer(vector<float> &vertices, vector<float>& texCoords);
void generateBackground(vector<float> &vertices, vector<float>& texCoords);


void loadFloor();
void loadPlayer();
void loadPlatform();
void loadBackground();


void loadSurfaceOfRevolution() 
{
/*------------------------------CREATE GEOMETRY-------------------------------*/
	
	generateFloor(floorVerts, floorTexCoords);
	generatePlatform(platVerts, platTexCoords);
	generatePlayer(playerVerts, playerTexCoords);
	generateBackground(backgroundVerts, backgroundTexCoords);

	loadBackground();
	loadFloor();
	loadPlayer();
	loadPlatform();
	

}

void loadUniforms(GLuint shader_programme)
{
	// Choose vertex and fragment shaders to use as well as view and proj matrices.
	int model_mat_location = glGetUniformLocation (shader_programme, "model_mat");
	int view_mat_location  = glGetUniformLocation (shader_programme, "view_mat");
	int proj_mat_location  = glGetUniformLocation (shader_programme, "proj_mat");
	int model_player_location = glGetUniformLocation (shader_programme, "model_player");
	
	glUniform1f(glGetUniformLocation(shader_programme, "uv_x"), uv_x);
	glUniform1f(glGetUniformLocation(shader_programme, "uv_y"), uv_y);
	glUniform1f(glGetUniformLocation(shader_programme, "nx_frames"), nx_frames);
	glUniform1f(glGetUniformLocation(shader_programme, "ny_frames"), ny_frames);
	

	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, proj_mat.m);
	glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, model_mat.m);
	glUniformMatrix4fv (model_player_location, 1, GL_FALSE, model_player.m);

	GLint textureUniform = glGetUniformLocation(shader_programme, "textureSampler");
	glUniform1i(textureUniform, 0);
}

void drawStage(GLuint shader_programme)
{	
/*---------------------------SET RENDERING DEFAULTS---------------------------*/


	
	// WRITE CODE TO LOAD OTHER UNIFORM VARIABLES LIKE FLAGS FOR ENABLING OR DISABLING CERTAIN FUNCTIONALITIES


	glBindVertexArray(floorVao);
	

	GLint textureN = glGetUniformLocation(shader_programme, "textureNum");
	glUniform1i(textureN, 1);

	GLint isPlayer_loc = glGetUniformLocation(shader_programme, "isPlayer");
	glUniform1i(isPlayer_loc, 0);

    glDrawArrays(GL_TRIANGLES, 0, floorVerts.size());


	glBindVertexArray(platVao);
    glDrawArrays(GL_TRIANGLES, 0, platVerts.size());

}

void drawBackGround(GLuint shader_programme)
{
	glBindVertexArray(backVao);
	GLint textureN = glGetUniformLocation(shader_programme, "textureNum");
	glUniform1i(textureN, 2);

	GLint isPlayer_loc = glGetUniformLocation(shader_programme, "isPlayer");
	glUniform1i(isPlayer_loc, 0);


    glDrawArrays(GL_TRIANGLES, 0, backgroundVerts.size());
}

void drawPlayer(GLuint shader_programme)
{

	// WRITE CODE TO LOAD OTHER UNIFORM VARIABLES LIKE FLAGS FOR ENABLING OR DISABLING CERTAIN FUNCTIONALITIES
	glBindVertexArray(playerVao);

	GLint textureN = glGetUniformLocation(shader_programme, "textureNum");
	glUniform1i(textureN, 0);

	GLint isPlayer_loc = glGetUniformLocation(shader_programme, "isPlayer");
	glUniform1i(isPlayer_loc, 1);
	
    glDrawArrays(GL_TRIANGLES, 0, playerVerts.size());

}


	
void keyboardFunction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// MODIFY THIS FUNCTION FOR KEYBOARD INTERACTIVITY
	//GLFW Reference Links:
	// Callback Example: https://www.glfw.org/docs/3.3/input_guide.html#input_key
	// List of Keys: https://www.glfw.org/docs/3.3/group__keys.html
	float movementSpeed = 0.00125f;
    if (key == GLFW_KEY_W)
    {
		 if (action == GLFW_PRESS) {
            isW = true;
			jumpTrigger = true;
			spriteY = movementSpeed;
        } else if (action == GLFW_RELEASE) {
            isW = false;
			spriteY = 1;
		}

	}
	 if (key == GLFW_KEY_S)
    {

		 if (action == GLFW_PRESS) {
            isS = true;
			spriteY = -movementSpeed;
        } else if (action == GLFW_RELEASE) {
            isS = false;
			spriteY = 0;
		}
	}
	 if (key == GLFW_KEY_A)
    {

		 if (action == GLFW_PRESS) {
            isA = true;
			isD = false;
			spriteX = -movementSpeed;
        } else if (action == GLFW_RELEASE) {
            isA = false;
			spriteX = 0;
		}
	}
	 if (key == GLFW_KEY_D)
    {
		 if (action == GLFW_PRESS) {
            isD = true;
			isA = false;
			spriteX = movementSpeed;
        } else if (action == GLFW_RELEASE) {
            isD = false;
			spriteX = 0;
		}
	}

        
	if (GLFW_PRESS == glfwGetKey (g_window, GLFW_KEY_ESCAPE)) {
		// Close window when esacape is pressed
			glfwSetWindowShouldClose (g_window, 1);
	}

}


void generateFloor(vector<float> &vertices, vector<float> &texCoords)
{	
	int offsetY = -1;
	float height = 0.2;
	float x = 2.5;
	
	
	//face 1, vertex 1
	vertices.push_back(-x);
	vertices.push_back(0 + offsetY);
	vertices.push_back(0);
	//face 1, vertex 2
	vertices.push_back(x);
	vertices.push_back(0 + offsetY);
	vertices.push_back(0);
	//face 1, vertex 3
	vertices.push_back(-x);
	vertices.push_back(height + offsetY);
	vertices.push_back(0);

	//face 2, vertex 1
	vertices.push_back(-x);
	vertices.push_back(height + offsetY);
	vertices.push_back(0);
	//face 2, vertex 2
	vertices.push_back(x);
	vertices.push_back(0 + offsetY);
	vertices.push_back(0);
	//face 2, vertex 3
	vertices.push_back(x);
	vertices.push_back(height + offsetY);
	vertices.push_back(0);

	texCoords.push_back(0.0f); // Texture coordinate for vertex 1 of face 1 (bottom-left)
    texCoords.push_back(0.0f);
    texCoords.push_back(5.0f); // Texture coordinate for vertex 2 of face 1 (bottom-right)
    texCoords.push_back(0.0f);
    texCoords.push_back(0.0f); // Texture coordinate for vertex 3 of face 1 (top-left)
    texCoords.push_back(1.0f);
    // Texture coordinate for vertex 1 of face 2 (top-left)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.0f);
    texCoords.push_back(5.0f); // Texture coordinate for vertex 2 of face 2 (bottom-right)
    texCoords.push_back(0.0f);
    texCoords.push_back(5.0f); // Texture coordinate for vertex 3 of face 2 (top-right)
    texCoords.push_back(1.0f);


	

}


void generatePlatform(vector<float> &vertices, vector<float> &texCoords)
{	
	float offsetY = -0.25;
	int offsetX=0;
	float x = 0.75;
	float height = 0.2;
	//face 1, vertex 1
	vertices.push_back(-x + offsetX);
	vertices.push_back(0 + offsetY);
	vertices.push_back(0);
	//face 1, vertex 2
	vertices.push_back(x+ offsetX);
	vertices.push_back(0+ offsetY);
	vertices.push_back(0);
	//face 1, vertex 3
	vertices.push_back(-x+ offsetX);
	vertices.push_back(height + offsetY);
	vertices.push_back(0);

	//face 2, vertex 1
	vertices.push_back(-x+ offsetX);
	vertices.push_back(height + offsetY);
	vertices.push_back(0);
	//face 2, vertex 2
	vertices.push_back(x+ offsetX);
	vertices.push_back(0+ offsetY);
	vertices.push_back(0);
	//face 2, vertex 3
	vertices.push_back(x+ offsetX);
	vertices.push_back(height + offsetY);
	vertices.push_back(0);

	texCoords.push_back(0.0f); // Texture coordinate for vertex 1 of face 1 (bottom-left)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.5f); // Texture coordinate for vertex 2 of face 1 (bottom-right)
    texCoords.push_back(0.0f);
    texCoords.push_back(0.0f); // Texture coordinate for vertex 3 of face 1 (top-left)
    texCoords.push_back(1.0f);
    // Texture coordinate for vertex 1 of face 2 (top-left)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.0f);
    texCoords.push_back(1.5f); // Texture coordinate for vertex 2 of face 2 (bottom-right)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.5f); // Texture coordinate for vertex 3 of face 2 (top-right)
    texCoords.push_back(1.0f);

}

void generatePlayer(vector<float> &vertices, vector<float> &texCoords)
{
	float offsetY = -0.901f;
	float height = 0.62f;
	float x = height/2.0f;

	//face 1, vertex 1
	
	vertices.push_back(-x);
	vertices.push_back(0 + offsetY);
	vertices.push_back(playerZ);
	//face 1, vertex 2
	vertices.push_back(x);
	vertices.push_back(0+ offsetY);
	vertices.push_back(playerZ);
	//face 1, vertex 3
	vertices.push_back(-x);
	vertices.push_back(height + offsetY);
	vertices.push_back(playerZ);

	//face 2, vertex 1
	vertices.push_back(-x);
	vertices.push_back(height + offsetY);
	vertices.push_back(playerZ);
	//face 2, vertex 2
	vertices.push_back(x);
	vertices.push_back(0+ offsetY);
	vertices.push_back(playerZ);
	//face 2, vertex 3
	vertices.push_back(x);
	vertices.push_back(height + offsetY);
	vertices.push_back(playerZ);

	texCoords.push_back(0.0f); // Texture coordinate for vertex 1 of face 1 (bottom-left)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.0f); // Texture coordinate for vertex 2 of face 1 (bottom-right)
    texCoords.push_back(0.0f);
    texCoords.push_back(0.0f); // Texture coordinate for vertex 3 of face 1 (top-left)
    texCoords.push_back(1.0f);
    // Texture coordinate for vertex 1 of face 2 (top-left)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.0f);
    texCoords.push_back(1.0f); // Texture coordinate for vertex 2 of face 2 (bottom-right)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.0f); // Texture coordinate for vertex 3 of face 2 (top-right)
    texCoords.push_back(1.0f);


}

void generateBackground(vector<float> &vertices, vector<float>& texCoords)
{

	float offsetY = -1.9;
	float height = 3.5f;
	float x = 16.0f / 4.0f;
	//face 1, vertex 1
	
	vertices.push_back(-x);
	vertices.push_back(0 + offsetY);
	vertices.push_back(backZ);
	//face 1, vertex 2
	vertices.push_back(x);
	vertices.push_back(0+ offsetY);
	vertices.push_back(0);
	//face 1, vertex 3
	vertices.push_back(-x);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);

	//face 2, vertex 1
	vertices.push_back(-x);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);
	//face 2, vertex 2
	vertices.push_back(x);
	vertices.push_back(0+ offsetY);
	vertices.push_back(backZ);
	//face 2, vertex 3
	vertices.push_back(x);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);

	texCoords.push_back(0.0f); // Texture coordinate for vertex 1 of face 1 (bottom-left)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.0f); // Texture coordinate for vertex 2 of face 1 (bottom-right)
    texCoords.push_back(0.0f);
    texCoords.push_back(0.0f); // Texture coordinate for vertex 3 of face 1 (top-left)
    texCoords.push_back(1.0f);
    // Texture coordinate for vertex 1 of face 2 (top-left)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.0f);
    texCoords.push_back(1.0f); // Texture coordinate for vertex 2 of face 2 (bottom-right)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.0f); // Texture coordinate for vertex 3 of face 2 (top-right)
    texCoords.push_back(1.0f);

}



void loadFloor()
{
	// VAO -- vertex attribute objects bundle the various things associated with vertices
	glGenVertexArrays (1, &floorVao);   // generating and binding is common pattern in OpenGL
	glBindVertexArray (floorVao);       // basically setting up memory and associating it

	// VBO -- vertex buffer object to contain coordinates
	// MODIFY THE FOLLOWING BLOCK OF CODE APPRORIATELY FOR YOUR SURFACE OF REVOLUTION
	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, floorVerts.size() * sizeof(float), floorVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint texcoords_vbo;
    glGenBuffers(1, &texcoords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
    glBufferData(GL_ARRAY_BUFFER, floorTexCoords.size() * sizeof(float), floorTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
}

void loadPlatform()
{
	// VAO -- vertex attribute objects bundle the various things associated with vertices
	glGenVertexArrays (1, &platVao);   // generating and binding is common pattern in OpenGL
	glBindVertexArray (platVao);       // basically setting up memory and associating it

	// VBO -- vertex buffer object to contain coordinates
	// MODIFY THE FOLLOWING BLOCK OF CODE APPRORIATELY FOR YOUR SURFACE OF REVOLUTION
	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, platVerts.size() * sizeof(float), platVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint texcoords_vbo;
    glGenBuffers(1, &texcoords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
    glBufferData(GL_ARRAY_BUFFER, platTexCoords.size() * sizeof(float), platTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

}

void loadPlayer()
{
	// VAO -- vertex attribute objects bundle the various things associated with vertices

	glGenVertexArrays (1, &playerVao);   // generating and binding is common pattern in OpenGL
	glBindVertexArray (playerVao);       // basically setting up memory and associating it

	// VBO -- vertex buffer object to contain coordinates
	// MODIFY THE FOLLOWING BLOCK OF CODE APPRORIATELY FOR YOUR SURFACE OF REVOLUTION
	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, playerVerts.size() * sizeof(float), playerVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint texcoords_vbo;
    glGenBuffers(1, &texcoords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
    glBufferData(GL_ARRAY_BUFFER, playerTexCoords.size() * sizeof(float), playerTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

}

void loadBackground()
{
	// VAO -- vertex attribute objects bundle the various things associated with vertices
	glGenVertexArrays (1, &backVao);   // generating and binding is common pattern in OpenGL
	glBindVertexArray (backVao);       // basically setting up memory and associating it

	// VBO -- vertex buffer object to contain coordinates
	// MODIFY THE FOLLOWING BLOCK OF CODE APPRORIATELY FOR YOUR SURFACE OF REVOLUTION
	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, backgroundVerts.size() * sizeof(float), backgroundVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint texcoords_vbo;
    glGenBuffers(1, &texcoords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
    glBufferData(GL_ARRAY_BUFFER, backgroundTexCoords.size() * sizeof(float), backgroundTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
}