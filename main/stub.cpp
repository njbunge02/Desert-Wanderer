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

using namespace std;

vector<float> floorVerts;
vector<float> floorNormals;
vector<float> floorTexCoords;

vector<float> playerVerts;
vector<float> playerNormals;
vector<float> playerTexCoords;

vec3 lightPosition(0.0f,0.0f,1.0f);
float ambient = 0.5;
float specularHighlight = 5;

bool diffuseOn = true;
bool specularOn = true;
bool textureOn = true;

int pointCount;


void generateFloor(vector<float> &vertices, vector<float> &normals, vector<float>& texCoords);
void loadFloor();


void loadSurfaceOfRevolution() 
{
/*------------------------------CREATE GEOMETRY-------------------------------*/
	
	
	float angle;
	float scale;
	int steps;
	
	scale = 3.0;
	angle = 11.25;
	steps = 75;
	

	generateFloor(floorVerts, floorNormals, floorTexCoords);
	loadFloor();
	
	

}

void loadUniforms(GLuint shader_programme)
{	
/*---------------------------SET RENDERING DEFAULTS---------------------------*/

	// Choose vertex and fragment shaders to use as well as view and proj matrices.
	int model_mat_location = glGetUniformLocation (shader_programme, "model_mat");
	int view_mat_location  = glGetUniformLocation (shader_programme, "view_mat");
	int proj_mat_location  = glGetUniformLocation (shader_programme, "proj_mat");
	
	glUniformMatrix4fv (view_mat_location, 1, GL_FALSE, view_mat.m);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, proj_mat.m);
	glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, model_mat.m);
	
	// WRITE CODE TO LOAD OTHER UNIFORM VARIABLES LIKE FLAGS FOR ENABLING OR DISABLING CERTAIN FUNCTIONALITIES


	GLuint lightColorLoc = glGetUniformLocation(shader_programme, "lightColor");
	GLuint lightDirectionLoc = glGetUniformLocation(shader_programme, "lightDirection");
	GLuint ambienceLoc = glGetUniformLocation(shader_programme, "ambience");
	GLint textureUniform = glGetUniformLocation(shader_programme, "textureSampler");
	GLint specLoc = glGetUniformLocation(shader_programme, "specularHigh");


	GLuint diffOn = glGetUniformLocation(shader_programme, "diffuseOn");
	GLuint specOn = glGetUniformLocation(shader_programme, "specularOn");
	GLint textOn = glGetUniformLocation(shader_programme, "textureOn");


	 vec3 lightColorValue = vec3(1.0f, 1.0f, 1.0f);
	 glUniform3f(lightColorLoc, lightColorValue.v[0], lightColorValue.v[1], lightColorValue.v[2]);

	vec3 lightDirectionValue = lightPosition; 
	lightDirectionValue = normalise(lightDirectionValue);

    glUniform3f(lightDirectionLoc, lightDirectionValue.v[0], lightDirectionValue.v[1], lightDirectionValue.v[2]);

    glUniform1f(ambienceLoc, ambient);
	 glUniform1f(specLoc, specularHighlight);

	glUniform1i(textureUniform, 0);


	glUniform1i(diffOn, diffuseOn);
	glUniform1i(specOn, specularOn);
	glUniform1i(textOn, textureOn);




}

void drawScene()
{

    // Modify this line appropriately based on your sphere data
    glDrawArrays(GL_TRIANGLES, 0, floorVerts.size());


}
	
void keyboardFunction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// MODIFY THIS FUNCTION FOR KEYBOARD INTERACTIVITY
	//GLFW Reference Links:
	// Callback Example: https://www.glfw.org/docs/3.3/input_guide.html#input_key
	// List of Keys: https://www.glfw.org/docs/3.3/group__keys.html
	
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
		lightPosition.v[1] += 0.5;
	}
	 if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
		lightPosition.v[1] -= 0.5;
	}
	 if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
		lightPosition.v[0] -= 0.5;
	}
	 if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
		lightPosition.v[0] += 0.5;
	}

        
	if (GLFW_PRESS == glfwGetKey (g_window, GLFW_KEY_ESCAPE)) {
		// Close window when esacape is pressed
			glfwSetWindowShouldClose (g_window, 1);
	}

}


void generateFloor(vector<float> &vertices, vector<float> &normals, vector<float>& texCoords)
{	
	int offsetY = -1;
	float x = 2.5;
	//face 1, vertex 1
	vertices.push_back(-x);
	vertices.push_back(0 + offsetY);
	vertices.push_back(0);
	//face 1, vertex 2
	vertices.push_back(x);
	vertices.push_back(0+ offsetY);
	vertices.push_back(0);
	//face 1, vertex 3
	vertices.push_back(-x);
	vertices.push_back(0.1+ offsetY);
	vertices.push_back(0);

	//face 2, vertex 1
	vertices.push_back(-x);
	vertices.push_back(0.1+ offsetY);
	vertices.push_back(0);
	//face 2, vertex 2
	vertices.push_back(x);
	vertices.push_back(0+ offsetY);
	vertices.push_back(0);
	//face 2, vertex 3
	vertices.push_back(x);
	vertices.push_back(0.1+ offsetY);
	vertices.push_back(0);


	vec3 point;
	for (int i = 0; i < vertices.size(); i += 3)
	{
		point.v[0] = vertices[i];
		point.v[1] = vertices[i+1];
		point.v[2] = vertices[i+2];

		point = normalise(point);

		normals.push_back(point.v[0]);
		normals.push_back(point.v[1]);
		normals.push_back(point.v[2]);
	}


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
	GLuint vao;
	glGenVertexArrays (1, &vao);   // generating and binding is common pattern in OpenGL
	glBindVertexArray (vao);       // basically setting up memory and associating it

	// VBO -- vertex buffer object to contain coordinates
	// MODIFY THE FOLLOWING BLOCK OF CODE APPRORIATELY FOR YOUR SURFACE OF REVOLUTION
	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, floorVerts.size() * sizeof(float), floorVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// VBO -- normals -- needed for shading calcuations
	// ADD CODE TO POPULATE AND LOAD PER-VERTEX SURFACE NORMALS  
	// [HINT] Vertex normals are organized in same order as that for vertex coordinates

	GLuint normals_vbo;
    glGenBuffers(1, &normals_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
    glBufferData(GL_ARRAY_BUFFER, floorNormals.size() * sizeof(float), floorNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
	


    // VBO -- vt -- texture coordinates
	// ADD CODE TO POPULATE AND LOAD PER-VERTEX TEXTURE COORDINATES  
	// [HINT] texture coordinates are organized in same order as that for vertex coordinates
	// [HINT] there are two texture coordinates instead of three vertex coordinates for each vertex
	GLuint texcoords_vbo;
    glGenBuffers(1, &texcoords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
    glBufferData(GL_ARRAY_BUFFER, floorTexCoords.size() * sizeof(float), floorTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
}