/******************************************************************************|
| CPSC 4050/6050 Computer Garphics Assignment 5, Daljit Singh Dhillon, 2020    |
| Modified from:                                                               |
|                                                                              |
| DPA 8090 Example Code, Eric Patterson, 2017                                  |
|                                                                              |
| This is mostly plain C but uses a few things from C++ so needs C++ compiler. |
| Some OpenGL setup code here and obj_parser, math_funcs, and gl_utils         |
| are from Angton Gerdelan and "Anton's OpenGL 4 Tutorials."                   |
| http://antongerdelan.net/opengl/                                             |
| Email: anton at antongerdelan dot net                                        |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
|******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#include <math.h>
#include <time.h>

#include <GL/glew.h>       // Include GLEW (or new version of GL if on Windows).
#include <GLFW/glfw3.h>    // GLFW helper library.

#include "maths_funcs.h"   // Anton's maths functions.
#include "gl_utils.h"      // Anton's opengl functions and small utilities like logs
#include "obj_parser.h"    // Anton's little Wavefront .obj mesh loader


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"     // Sean Barrett's image loader with Anton's load_texture()

#define _USE_MATH_DEFINES
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

// The view and proj matrices below make-up the camera position, orientation, fov, etc.
// The model matrices moves your surface of revolution to the world coordinate system
extern mat4 view_mat;
extern mat4 proj_mat;
extern mat4 model_mat;
extern mat4 model_player;

extern float nx_frames, ny_frames;
extern float uv_x, uv_y;
double numXInAnimation = 6.0f;

extern bool isW;
extern bool isA;
extern bool isS;
extern bool isD;

// the vector below indicates camra placement. 
//It looks at (0,0,0) with (0,1,0) as the up-vector
vec3 cam_pos (0.0f, 0.0f, 5.0f);

// Below are the declarations for three functions from stub.cpp 
void loadSurfaceOfRevolution();

void loadUniforms(GLuint shader_programme);
void drawStage(GLuint shader_programme);
void drawPlayer(GLuint shader_programme);
void keyboardFunction(GLFWwindow* window, int key, int scancode, int action, int mods);


// Forward Declaration
// this function loads a .jpg or .png file into a previously activated texture map
bool load_texture (const char* file_name, GLuint* tex);


/******************************************************************************|
| argc: number of input command-line parameters                                |
| argv: array of input command-line character strings                          |
| EXPECTED INPUT                                                               |
|                                                                              |
| argv[0]: automatically asigned the program name cg05                         |
| argv[1]: Vertex Shader Filename (first argument at command-line)             |
| argv[2]: Fragment Shader Filename (second argument at command-line)          |
| argv[3]: First Texture Filename (third argument at command-line)             |
| argv[4]: Second Texture Filename(optional fourth argument at command-line)   |
|******************************************************************************/
const float frameHeight = 1.0f / nx_frames;
int main (int argc, char *argv[]) {
/*--------------------------------START OPENGL--------------------------------*/
	assert (restart_gl_log ());
	assert (start_gl ());        // Start glfw window with GL context within.

	glfwSetKeyCallback(g_window, keyboardFunction);

/*------------------------------CREATE GEOMETRY-------------------------------*/
	loadSurfaceOfRevolution();

/*-------------------------------CREATE SHADERS-------------------------------*/
    // The vertex shader program generally acts to transform vertices.
    // The fragment shader is where we'll do the actual "shading."
	GLuint shader_programme = create_programme_from_files (
		"vs.glsl", "frag.glsl"
	);

	glUseProgram (shader_programme);
	

/*-------------------------------SETUP TEXTURES-------------------------------*/
	// load textures
	unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(GL_TRUE);
    unsigned char *img_data = stbi_load("./spritesheet.png", &width, &height, &nrChannels, 0);
    if(!img_data) {
        fprintf(stderr, "Failed loading image!\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(img_data);






	/*unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width1, height1, nrChannels1;
    unsigned char *img_data1 = stbi_load("./Meow Knight/Meow-Knight_Run.png", &width1, &height1, &nrChannels1, 0);
    if(!img_data1) {
        fprintf(stderr, "Failed loading image!\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data1);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(img_data1);*/
	


	
/*-------------------------------CREATE CAMERA--------------------------------*/

        // This sets up the matrix transformations that act as the camera lens
        // and sensor would to transform vertices from view space.
	float near = 0.1f;   // clipping planes
	float far = 1000.0f; 
	float fovy = 35.0f;  // vertical field of view, horiz calculated for aspect
	float aspect = (float)g_gl_width / (float)g_gl_height;      // aspect ratio
	proj_mat = perspective (fovy, aspect, near, far);

	mat4 T = translate (
		identity_mat4 (), vec3 (-cam_pos.v[0], -cam_pos.v[1], -cam_pos.v[2])
	);
	
	// would usually use inverse camera orientation with position to construct
	// view matrix, but for simplicity since camera axis-aligned, not needed here
	view_mat =  T; 
	
/*---------------------------SET RENDERING DEFAULTS---------------------------*/
	// The model matrix stores the position and orientation transformations for your SoR

	model_player = identity_mat4();
	model_mat = identity_mat4();

	double frames_ps = 10.0f;


	// Setup basic GL display attributes.	
	glEnable (GL_DEPTH_TEST);   // enable depth-testing
	glDepthFunc (GL_LESS);      // depth-testing interprets a smaller value as "closer"
	glEnable (GL_CULL_FACE);    // cull face
	glCullFace (GL_BACK);       // cull back face
	glFrontFace (GL_CCW);       // set counter-clock-wise vertex order to mean the front
	glClearColor (0.1, 0.1, 0.1, 1.0);   // non-black background to help spot mistakes
	glViewport (0, 0, g_gl_width, g_gl_height); // make sure correct aspect ratio
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double now_time, old_time, time_delta;
	now_time = old_time = 0.0f;

/*-------------------------------RENDERING LOOP-------------------------------*/
	while (!glfwWindowShouldClose (g_window)) {
		// update timers
		now_time = glfwGetTime();
		time_delta = now_time - old_time;
		bool down = true;
		_update_fps_counter (g_window);
        now_time = glfwGetTime();
        time_delta = now_time - old_time;
        if(time_delta >= 1.0f / frames_ps) {
            old_time = now_time;
            time_delta = 0.0f;
            uv_x += 1.0f;
            if(uv_x >= numXInAnimation) {
                uv_x = 0.0f;
            }
        }
		
		vec3 position;
    	position.v[0] = model_player.m[12]; // X component of translation
   		position.v[1] = model_player.m[13]; // Y component of translation
    	position.v[2] = model_player.m[14]; // Z component of translation


		if (isW)
			model_player = translate(model_player, vec3(0.0f, 0.001f, 0.0f));
		if (isA && position.v[0] > -2)
			model_player = translate(model_player, vec3(-0.001f, 0.0f, 0.0f));
		if (isD && position.v[0] < 2)
			model_player = translate(model_player, vec3(0.001f, 0.0f, 0.0f));
		if (isS && position.v[1] > -0.9f)
			{
			model_player = translate(model_player, vec3(0.0f, -0.001f, 0.0f));
			}



		// clear graphics context
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		// setup shader use	
		glUseProgram (shader_programme);


		// load uniform variables for shaders
		// YOU HAVE TO IMPLEMENT THIS FUNCTION IN stub.cpp	

		
		
		// The following function will actually draw your previously dispatched/loaded Surface of Revolution
		// YOU HAVE TO IMPLEMENT THIS FUNCTION IN stub.cpp	
		
		loadUniforms(shader_programme);


		drawStage(shader_programme);
		drawPlayer(shader_programme);

		// update other events like input handling 
		glfwPollEvents ();

		aspect = (float)g_gl_width / (float)g_gl_height; // aspect ratio
		proj_mat = perspective (fovy, aspect, near, far);
	  
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers (g_window);
	}
	
	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}

// this function loads a .jpg or .png file into a previously activated texture map
bool load_texture (const char* file_name, GLuint* tex) {
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load (file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf (stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf (
			stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
		);
	}
	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}
	glGenTextures (1, tex);
	glBindTexture (GL_TEXTURE_2D, *tex);
	glTexImage2D (
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
	);
	glGenerateMipmap (GL_TEXTURE_2D);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	return true;
}
