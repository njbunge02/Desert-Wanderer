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

float nx_frames = 10.0f, ny_frames = 3.0f;
float uv_x = 1.0f, uv_y = 2.0f;
double numXInAnimation = 6.0f;

extern bool isW;
extern bool isA;
extern bool isS;
extern bool isD;
extern bool jumpTrigger;

//on if the sprite is idling
bool idleFlag;

extern float spriteX;
extern float spriteY;

//keeps count of cycles since key is released
int count;

//z coordinate of player and back
float playerZ = 0.01f;
float backZ = -0.01f;


// the vector below indicates camra placement. 
//It looks at (0,0,0) with (0,1,0) as the up-vector
vec3 cam_pos (0.0f, 0.0f, 5.0f);

// Below are the declarations for three functions from stub.cpp 
void loadSurfaceOfRevolution();

void loadUniforms(GLuint shader_programme);
void drawStage(GLuint shader_programme);
void drawPlayer(GLuint shader_programme);
void drawBackGround(GLuint shader_programme);
void keyboardFunction(GLFWwindow* window, int key, int scancode, int action, int mods);
void moveCameraRight(mat4& view_mat);
void moveCameraLeft(mat4& view_mat);


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


	unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width1, height1, nrChannels1;
    unsigned char *img_data1 = stbi_load("grass.png", &width1, &height1, &nrChannels1, 0);
    if(!img_data1) {
        fprintf(stderr, "Failed loading image!\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data1);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(img_data1);


	unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width2, height2, nrChannels2;
    unsigned char *img_data2 = stbi_load("back.jpg", &width2, &height2, &nrChannels2, 0);
    if(!img_data2) {
        fprintf(stderr, "Failed loading image!\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
	//loaded in as RGB instead of RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data2);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(img_data2);
	
	

	
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

float jumpVelocity = 0.0f; // Initial jump velocity
float gravity = 0.00005f; // Gravity acting on the player
bool first = true;	//the first time the jump loop is entered
bool OnPlat = false;	//true if player is on platform
bool afterJumpFlag = false;
bool fallFlag = false;

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
			{count = 0;
			idleFlag = false;
			}
		else if (isA)
			{idleFlag = false;
			GLint textScale = glGetUniformLocation(shader_programme, "reverse");
			glUniform1i(textScale, 1);
			count = 0;
			if (!jumpTrigger)	//changes animation if its not jumping
			{uv_y = 0.0f;
			numXInAnimation = 8.0f;}
			}
		else if (isD){
			GLint textScale = glGetUniformLocation(shader_programme, "reverse");
			glUniform1i(textScale, 0);
		count = 0;
			if (!jumpTrigger) //changes animation if its not jumping
			{uv_y = 0.0f;
			numXInAnimation = 8.0f;
			idleFlag = false;
		}
		}
		else if (isS)
		{
			idleFlag = false;
			fallFlag = true;
		}else 
		{

		count++;
		}

		if (count > 20)	//makes sure 50 cycles pass before it switches to idle animation after key release
		{	uv_y = 2.0f;
		if (!idleFlag)
		{
			uv_x = 0.0f;
			idleFlag = true;
		}
			numXInAnimation = 6.0f;
		}


	//moves player left and right
		if (position.v[0] + spriteX > -3.7f && position.v[0] + spriteX < 3.7f )	//determines if player is in border bounds
		{
			if ((position.v[0] > -0.75f && position.v[0] < 0.75f && position.v[1] < 0.70f && position.v[1] > 0.25f) && !fallFlag)	//determines if player is in the platform 
			{
				if(abs(position.v[0] + 0.75f) <= abs(position.v[0] - 0.75))	//determines which side the player is closest to and moves them out of the platform
				{
					model_player = translate(model_player, vec3(-0.002f, 0.0f, 0.0f));
				} else 
				{
					model_player = translate(model_player, vec3(0.002f, 0.0f, 0.0f));
				}
			} else
			{
			model_player = translate(model_player, vec3(spriteX, 0.0f, 0.0f));
			if (position.v[0] > 2.0f || position.v[0] < -2.0f)
			{
				if (isD)
					moveCameraRight(view_mat);
				else if (isA)
					moveCameraLeft(view_mat);
			}
			}
		}
	

	//jump animation
		if (jumpTrigger && !fallFlag)
		{
			afterJumpFlag = true;
			if (first)	//if first time entering jump loop
				{
					jumpVelocity = 0.01f;	//set it to intial velocity 
					first = false;
				}
			
			uv_y = 1.0f;
			numXInAnimation = 10.0f;
			jumpVelocity -= gravity;
			if (position.v[1] + jumpVelocity > 0 )	//if sprite is above floor
			{
				if (position.v[0] > -0.75f && position.v[0] < 0.75f && !OnPlat && position.v[1] > 0.75f)	//if sprite is not inbetween the platform 
				{
					if (position.v[1] + jumpVelocity > 0.75f)	// if the player is above the platform
					{
						model_player = translate(model_player, vec3(0.0f, jumpVelocity, 0.0f));
					} else
					{
						OnPlat = true;
						jumpTrigger = false;
						jumpVelocity = 0;
						first = true;
					}
			
				}else{
					OnPlat = false;
					model_player = translate(model_player, vec3(0.0f, jumpVelocity, 0.0f));
				}
			}else
			{
				jumpTrigger = false;
				jumpVelocity = 0;
				if (isA)
				{
				GLint textScale = glGetUniformLocation(shader_programme, "reverse");
				glUniform1i(textScale, 1);
				uv_y = 0.0f;
				numXInAnimation = 8.0f;
				} else if (isD){
				GLint textScale = glGetUniformLocation(shader_programme, "reverse");
				glUniform1i(textScale, 0);
				uv_y = 0.0f;
				numXInAnimation = 8.0f;
				}

				if (afterJumpFlag)
				{
					uv_x = 0.0;
					afterJumpFlag = false;
				}
				
			}
		} else if (position.v[1] + jumpVelocity > 0 && (position.v[0] < -0.75f || position.v[0] > 0.75f || fallFlag))//if there is no jump animation and normal gravity should apply
		{
			jumpVelocity -= gravity;
			first = true;
			model_player = translate(model_player, vec3(0.0f, jumpVelocity, 0.0f));
		}else if ((position.v[0] > -0.75f && position.v[0] < 0.75f))
		{
			
			first = true;
			if ((position.v[1] + jumpVelocity > 0.75f || position.v[1] + jumpVelocity < 0.25f) && position.v[1] + jumpVelocity > 0)
				{
				jumpVelocity -= gravity;
				model_player = translate(model_player, vec3(0.0f, jumpVelocity, 0.0f));}
			else
			{
				fallFlag = false;
				jumpTrigger = false;
			}
		} else
		{
			jumpTrigger = false;
		}

		
		


		// clear graphics context
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		// setup shader use	
		glUseProgram (shader_programme);


	
	loadUniforms(shader_programme);
   
	glUniform1i(glGetUniformLocation(shader_programme, "textureSampler"), 0); 

    // Bind the texture to GL_TEXTURE0 (if that's the texture unit index you're using)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture2); // Bind your desired texture to the unit
	drawBackGround(shader_programme);



   glUniform1i(glGetUniformLocation(shader_programme, "textureSampler"), 0);

    // Bind the texture to GL_TEXTURE0 (if that's the texture unit index you're using)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1); // Bind your desired texture to the unit

	
	drawStage(shader_programme);
	glUniform1i(glGetUniformLocation(shader_programme, "textureSampler"), 0); 
    // Bind the texture to GL_TEXTURE0 (if that's the texture unit index you're using)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture); // Bind your desired texture to the unit

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

void moveCameraRight(mat4& view_mat) {

    // Update the view matrix with the new camera position
	 vec3 right = vec3(view_mat.m[0], view_mat.m[4], view_mat.m[8]); // Extract the camera's right vector
    vec3 translation = right * -0.0015f; // Define the amount of translation (adjust as needed)

    // Translate the camera position by the calculated amount
   if (view_mat.m[12] > -1.2f)
	{
		 view_mat = translate(view_mat, translation);
	}

}

void moveCameraLeft(mat4& view_mat) {
    vec3 right = vec3(view_mat.m[0], view_mat.m[4], view_mat.m[8]); // Extract the right vector from the view matrix
    vec3 translation = right * 0.0015f; // Move the camera position to the left by the specified amount
	if (view_mat.m[12] < 1.2f)
	{
		 view_mat = translate(view_mat, translation);
	}
	
   
}

