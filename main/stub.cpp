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




//true if their respective key is pressed...false if released
bool isW;	
bool isA;	
bool isS;
bool isD;

//the relative change in x/y for the sprite in main
float spriteX;
float spriteY;

//triggers jump animation
bool jumpTrigger;


using namespace std;

//floor,platform,player,background vertices and texture coordinates
vector<float> floorVerts;
vector<float> floorTexCoords;

vector<float> platVerts;
vector<float> platTexCoords;

vector<float> skyVerts;
vector<float> skyTexCoords;

vector<float> mtnVerts;
vector<float> mtnTexCoords;

vector<float> backMtnVerts;
vector<float> backMtnTexCoords;

vector<float> foreGroundVerts;
vector<float> foreGroundTexCoords;


vector<float> playerVerts;
vector<float> playerTexCoords;

GLuint playerVao;
GLuint floorVao;
GLuint platVao;

GLuint skyVao;
GLuint mtnVao;
GLuint backMtnVao;
GLuint foreGroundVao;



//creates vertices and texture coordinates
void generateFloor(vector<float> &vertices, vector<float> &texCoords);
void generatePlatform(vector<float> &vertices, vector<float> &texCoords);
void generatePlayer(vector<float> &vertices, vector<float>& texCoords);

void generateSky(vector<float> &vertices, vector<float>& texCoords);
void generateMtn(vector<float> &vertices, vector<float>& texCoords);
void generateBackMtn(vector<float> &vertices, vector<float>& texCoords);
void generateForeGround(vector<float> &vertices, vector<float>& texCoords);


//creates their VBOs and VAOs
void loadFloor();
void loadPlayer();
void loadPlatform();

void loadSky();
void loadMtn();
void loadBackMtn();
void loadForeGround();



void loadSurfaceOfRevolution() 
{
/*------------------------------CREATE GEOMETRY-------------------------------*/
	
	generateFloor(floorVerts, floorTexCoords);
	generatePlatform(platVerts, platTexCoords);
	generatePlayer(playerVerts, playerTexCoords);
	generateSky(skyVerts, skyTexCoords);
	generateBackMtn(backMtnVerts, backMtnTexCoords);
	generateMtn(mtnVerts, mtnTexCoords);
	generateForeGround(foreGroundVerts, foreGroundTexCoords);

	loadForeGround();
	loadSky();
	loadBackMtn();
	loadMtn();

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


	


	glBindVertexArray(floorVao);	//floor VAO

	//changes the texture to the dirt block in frag shader
	GLint textureN = glGetUniformLocation(shader_programme, "textureNum");
	glUniform1i(textureN, 1);

	//changes is player value to false so the model matrix is not the player matrix
	GLint isPlayer_loc = glGetUniformLocation(shader_programme, "isPlayer");
	glUniform1i(isPlayer_loc, 0);
    
	//draws both floor and platform
	glDrawArrays(GL_TRIANGLES, 0, floorVerts.size());

	glBindVertexArray(platVao);
    glDrawArrays(GL_TRIANGLES, 0, platVerts.size());

}



void drawSky(GLuint shader_programme)
{
glBindVertexArray(skyVao);

	//changes the texture to the sunset in frag shader
	GLint textureN = glGetUniformLocation(shader_programme, "textureNum");
	glUniform1i(textureN, 2);

	//changes is player value to false so the model matrix is not the player matrix
	GLint isPlayer_loc = glGetUniformLocation(shader_programme, "isPlayer");
	glUniform1i(isPlayer_loc, 0);

	//draws background
    glDrawArrays(GL_TRIANGLES, 0, skyVerts.size());
}

void drawForeGround(GLuint shader_programme)
{
	glBindVertexArray(foreGroundVao);
//changes the texture to the sunset in frag shader
	GLint textureN = glGetUniformLocation(shader_programme, "textureNum");
	glUniform1i(textureN, 2);

	//changes is player value to false so the model matrix is not the player matrix
	GLint isPlayer_loc = glGetUniformLocation(shader_programme, "isPlayer");
	glUniform1i(isPlayer_loc, 0);

	//draws background
    glDrawArrays(GL_TRIANGLES, 0, foreGroundVerts.size());
}
void drawMtn(GLuint shader_programme)
{
glBindVertexArray(mtnVao);

	//changes the texture to the sunset in frag shader
	GLint textureN = glGetUniformLocation(shader_programme, "textureNum");
	glUniform1i(textureN, 2);

	//changes is player value to false so the model matrix is not the player matrix
	GLint isPlayer_loc = glGetUniformLocation(shader_programme, "isPlayer");
	glUniform1i(isPlayer_loc, 0);

	//draws background
    glDrawArrays(GL_TRIANGLES, 0, mtnVerts.size());
}

void drawBackMtn(GLuint shader_programme)
{
glBindVertexArray(backMtnVao);

	//changes the texture to the sunset in frag shader
	GLint textureN = glGetUniformLocation(shader_programme, "textureNum");
	glUniform1i(textureN, 2);

	//changes is player value to false so the model matrix is not the player matrix
	GLint isPlayer_loc = glGetUniformLocation(shader_programme, "isPlayer");
	glUniform1i(isPlayer_loc, 0);

	//draws background
    glDrawArrays(GL_TRIANGLES, 0, backMtnVerts.size());
}






void drawPlayer(GLuint shader_programme)
{


	glBindVertexArray(playerVao);

	//set to the sprite texture
	GLint textureN = glGetUniformLocation(shader_programme, "textureNum");
	glUniform1i(textureN, 0);

	//set to true because to make the model matrix the player matrix in frag shader
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

	float movementSpeed = 0.02f;	//how far the sprite moves per frame that key is pressed
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
	if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS) {
			isA = true;
		} else if (action == GLFW_RELEASE) {
			isA = false;
		}
	}

	if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS) {
			isD = true;
		} else if (action == GLFW_RELEASE) {
			isD = false;
		}
	}

	if (isA && !isD) {
		spriteX = -movementSpeed;
	} else if (isD && !isA) {
		spriteX = movementSpeed;
	} else {
		spriteX = 0;
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
	float x = 5;
	float  textureScale = 10.0f;
	
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
    texCoords.push_back(textureScale); // Texture coordinate for vertex 2 of face 1 (bottom-right)
    texCoords.push_back(0.0f);
    texCoords.push_back(0.0f); // Texture coordinate for vertex 3 of face 1 (top-left)
    texCoords.push_back(1.0f);
    // Texture coordinate for vertex 1 of face 2 (top-left)
    texCoords.push_back(0.0f);
    texCoords.push_back(1.0f);
    texCoords.push_back(textureScale); // Texture coordinate for vertex 2 of face 2 (bottom-right)
    texCoords.push_back(0.0f);
    texCoords.push_back(textureScale); // Texture coordinate for vertex 3 of face 2 (top-right)
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

void generateSky(vector<float> &vertices, vector<float>& texCoords)
{
	float offsetX = 1.0f;
	float offsetY = -1;
	float scale = 6.0f;
	float height = 3.0f * scale;
	float x = 4.0f * scale;
	float backZ = -30.0f;
	//face 1, vertex 1
	
	vertices.push_back(-1 * x + offsetX);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 1, vertex 2
	vertices.push_back(x+ offsetX);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 1, vertex 3
	vertices.push_back(-1 * x+ offsetX);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);

	//face 2, vertex 1
	vertices.push_back(-1 * x - 0.01f+ offsetX);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);
	//face 2, vertex 2
	vertices.push_back(x - 0.01f+ offsetX);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 2, vertex 3
	vertices.push_back(x+ offsetX);
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

void generateMtn(vector<float> &vertices, vector<float>& texCoords)
{

	float offsetY = -8.0f - 1.0f;
	float scale = 4.0f;
	float height = 3.0f * scale;
	float x = 4.0f * scale;
	float backZ = -20.0f;
	//face 1, vertex 1
	
	vertices.push_back(-1 * x);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 1, vertex 2
	vertices.push_back(x);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 1, vertex 3
	vertices.push_back(-1 * x);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);

	//face 2, vertex 1
	vertices.push_back(-1 * x - 0.01f);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);
	//face 2, vertex 2
	vertices.push_back(x - 0.01f);
	vertices.push_back(offsetY);
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

void generateBackMtn(vector<float> &vertices, vector<float>& texCoords)
{
	float offsetX = 6.0f;
	float offsetY = -2.5f - 1.0f;
	float scale = 2.2f;
	float height = 3.0f * scale;
	float x = 4.0f * scale;
	float backZ = -25.0f;
	//face 1, vertex 1
	
	vertices.push_back(-1 * x + offsetX);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 1, vertex 2
	vertices.push_back(x+ offsetX);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 1, vertex 3
	vertices.push_back(-1 * x+ offsetX);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);

	//face 2, vertex 1
	vertices.push_back(-1 * x - 0.01f+ offsetX);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);
	//face 2, vertex 2
	vertices.push_back(x - 0.01f+ offsetX);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 2, vertex 3
	vertices.push_back(x+ offsetX);
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



void generateForeGround(vector<float> &vertices, vector<float>& texCoords)
{
	float offsetX = 1.5f;
	float offsetY = -1.9f;
	float scale = 0.5f;
	float height = 3.0f * scale;
	float x = 4.0f * scale;
	float backZ = 1.0f;
	//face 1, vertex 1
	
	vertices.push_back(-1 * x + offsetX);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 1, vertex 2
	vertices.push_back(x+ offsetX);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 1, vertex 3
	vertices.push_back(-1 * x+ offsetX);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);

	//face 2, vertex 1
	vertices.push_back(-1 * x - 0.01f+ offsetX);
	vertices.push_back(height + offsetY);
	vertices.push_back(backZ);
	//face 2, vertex 2
	vertices.push_back(x - 0.01f+ offsetX);
	vertices.push_back(offsetY);
	vertices.push_back(backZ);
	//face 2, vertex 3
	vertices.push_back(x+ offsetX);
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

void loadSky()
{
	// VAO -- vertex attribute objects bundle the various things associated with vertices
	glGenVertexArrays (1, &skyVao);   // generating and binding is common pattern in OpenGL
	glBindVertexArray (skyVao);       // basically setting up memory and associating it

	// VBO -- vertex buffer object to contain coordinates

	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, skyVerts.size() * sizeof(float), skyVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint texcoords_vbo;
    glGenBuffers(1, &texcoords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
    glBufferData(GL_ARRAY_BUFFER, skyTexCoords.size() * sizeof(float), skyTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
}

void loadMtn()
{
	// VAO -- vertex attribute objects bundle the various things associated with vertices
	glGenVertexArrays (1, &mtnVao);   // generating and binding is common pattern in OpenGL
	glBindVertexArray (mtnVao);       // basically setting up memory and associating it

	// VBO -- vertex buffer object to contain coordinates

	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, mtnVerts.size() * sizeof(float), mtnVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint texcoords_vbo;
    glGenBuffers(1, &texcoords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
    glBufferData(GL_ARRAY_BUFFER, mtnTexCoords.size() * sizeof(float), mtnTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
}

void loadBackMtn()
{
	// VAO -- vertex attribute objects bundle the various things associated with vertices
	glGenVertexArrays (1, &backMtnVao);   // generating and binding is common pattern in OpenGL
	glBindVertexArray (backMtnVao);       // basically setting up memory and associating it

	// VBO -- vertex buffer object to contain coordinates

	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, backMtnVerts.size() * sizeof(float), backMtnVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint texcoords_vbo;
    glGenBuffers(1, &texcoords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
    glBufferData(GL_ARRAY_BUFFER, backMtnTexCoords.size() * sizeof(float), backMtnTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
}

void loadForeGround()
{
	// VAO -- vertex attribute objects bundle the various things associated with vertices
	glGenVertexArrays (1, &foreGroundVao);   // generating and binding is common pattern in OpenGL
	glBindVertexArray (foreGroundVao);       // basically setting up memory and associating it

	// VBO -- vertex buffer object to contain coordinates

	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, foreGroundVerts.size() * sizeof(float), foreGroundVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint texcoords_vbo;
    glGenBuffers(1, &texcoords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
    glBufferData(GL_ARRAY_BUFFER, foreGroundTexCoords.size() * sizeof(float), foreGroundTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
}