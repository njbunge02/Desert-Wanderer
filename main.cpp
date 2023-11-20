#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>   

#include <math.h>

#define WIDTH 800
#define HEIGHT 600

#define CP_WIDTH 10

int windowHandle;
int projectionMode = 0;
int perspectiveFOV = 45;
float cubeX = 0.0f, cubeY = 0.0f, cubeZ = 0.0f;
float rotateX = 0.0f, rotateY = 0.0f, rotateZ = 0.0f;
float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;

void drawPixel(int x, int y);

void drawCube();
void display();








void display(){
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    gluPerspective(perspectiveFOV, (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1, 100.0);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(3.0, 4.0, 5.0,  
              0.0, 0.0, 0.0,   
              0.0, 1.0, 0.0);

    glPushMatrix();




    
    glPopMatrix();

    glutSwapBuffers();
}

void initWindow()
/* Clear the image area, and set up the coordinate system */
{
    glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

   
    // Set up the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    
}
void keyboard(unsigned char key, int x, int y) {
    switch(key){

    }
}

int main (int argc, char *argv[])
{
/* This main function sets up the main loop of the program and continues the
   loop until the end of the data is reached.  Then the window can be closed
   using the escape key.						  */
	

		
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
	glutInitWindowSize(WIDTH,HEIGHT); 
	windowHandle = glutCreateWindow("Sprite Animation Project" );
    glEnable(GL_DEPTH_TEST);
    initWindow();	
	glutDisplayFunc(display);  
	glutIdleFunc(display);
    glutKeyboardFunc(keyboard);

		
	glutMainLoop(); 
	
    return 0;
}