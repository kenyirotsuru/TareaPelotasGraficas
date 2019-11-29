/*
 * Sergio Ruiz.
 *
 * TC3022. Computer Graphics Course.
 * Basic template OpenGL project.
 */

// Please don't change lines 9-31 (It helps me to grade)
#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
// For XCode only: New C++ terminal project. Build phases->Compile with libraries: add OpenGL and GLUT
// Import this whole code into a new C++ file (main.cpp, for example). Then run.
// Reference: https://en.wikibooks.org/wiki/OpenGL_Programming/Installation/Mac
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif
#ifdef _WIN32
// For VS on Windows only: Download CG_Demo.zip. UNZIP FIRST. Double click on CG_Demo/CG_Demo.sln
// Run
#include "freeglut.h"
#endif
#ifdef __unix__
// For Linux users only: g++ CG_Demo.cpp -lglut -lGL -o CG_Demo
// ./CG_Demo
// Reference: https://www.linuxjournal.com/content/introduction-opengl-programming
#include "GL/freeglut.h"
#include "GL/gl.h"
#endif

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <cstdlib>

#include "cParticle.h"
#include "Camera.hpp"
#define NUM_P 50

int gravityOn = 0;
Particle** particles; //double pointer for an array
float forces[NUM_P][3];
Camera* cam;
Camera* vfcCam;
Camera* currentCam; 


GLfloat*    light0_position;    //<-------------------------------Light 0    - location array
GLfloat*    light0_ambient;        //<-------------------------------Light 0    - ambient array
GLfloat*    light0_specular;    //<-------------------------------Light 0    - diffuse array
GLfloat*    light0_diffuse;        //<-------------------------------Light 0    - specular array


float randBetween(float min, float max){
    return min + (max-min) * (float)rand() / RAND_MAX;
}

void init()
{
    particles = new Particle*[NUM_P]; //Instantiate
    for (int p = 0; p < NUM_P; p++) {
        particles[p] = new Particle();
        
         particles[p]->pos[0] = randBetween(-10, 10);
         particles[p]->pos[1] = randBetween(-10, 10);
         particles[p]->pos[2] = randBetween(-10, 10);
        
        
        //particles[p]->pos[0] = particles[p]->pos[1] = particles[p]->pos[2] = 0;
        
        particles[p] -> oldPos[0] = particles[p] -> pos[0];
        particles[p] -> oldPos[1] = particles[p] -> pos[1];
        particles[p] -> oldPos[2] = particles[p] -> pos[2];
        
        particles[p] -> mass = randBetween(5,15);
        particles[p] -> radius = particles[p] -> mass / 10.0f;
        particles[p] -> area = 4.0f * M_PI * particles[p] -> radius * particles[p] -> radius;
        
        particles[p] -> diffuse[0] = randBetween(0, 1);
        particles[p] -> diffuse[1] = randBetween(0, 1);
        particles[p] -> diffuse[2] = randBetween(0, 1);
        
        particles[p] -> oDiffuse[0] = particles[p] -> diffuse[0];
        particles[p] -> oDiffuse[1] = particles[p] -> diffuse[1];
        particles[p] -> oDiffuse[2] = particles[p] -> diffuse[2];
        
        forces[p][0] = randBetween(-2, 2);
        forces[p][1] = randBetween(-2, 2);
        forces[p][2] = randBetween(-2, 2);
        particles[p] -> addForce(forces[p]);
    }
    //    p->pos[1] = 10;
    //    p -> oldPos[1] = 10;
    //    float gForce[3] = {0, -9.81f * p->mass, 0};
    //    p -> addForce(gForce);
    //->LIGHT 0 BEGINS
    light0_position = new GLfloat[4]; //<-------------------------Reserve memory
    light0_position[0] = 1.0f; //<--------------------------------L0x
    light0_position[1] = 1.0f; //<--------------------------------L0y
    light0_position[2] = 1.0f; //<--------------------------------L0z
    light0_position[3] = 0.0f; //<--------------------------------L0w
    
    /*
     Important: light position is L0(lx,ly,lz,lw).
     -If lw = 0, configures a directional light, and parameters lx, ly y lz define its direction.
     -If lw = 1, configures a point light, and parameters lx, ly y lz define its location.
     */
    
    light0_ambient = new GLfloat[4]; //<--------------------------Reserve memory
    light0_ambient[0] = 0.3f; //<---------------------------------Ia0r
    light0_ambient[1] = 0.3f; //<---------------------------------Ia0g
    light0_ambient[2] = 0.3f; //<---------------------------------Ia0b
    light0_ambient[3] = 1.0f; //<---------------------------------Ia0a
    
    light0_diffuse = new GLfloat[4]; //<--------------------------Reserve memory
    light0_diffuse[0] = 1.0f; //<---------------------------------Id0r
    light0_diffuse[1] = 1.0f; //<---------------------------------Id0g
    light0_diffuse[2] = 1.0f; //<---------------------------------Id0b
    light0_diffuse[3] = 1.0f; //<---------------------------------Id0a
    
    light0_specular = new GLfloat[4]; //<-------------------------Reserve memory
    light0_specular[0] = 1.0f; //<--------------------------------Is0r
    light0_specular[1] = 1.0f; //<--------------------------------Is0g
    light0_specular[2] = 1.0f; //<--------------------------------Is0b
    light0_specular[3] = 1.0f; //<--------------------------------Is0a
    
    //<-LIGHT 0 ENDS
    
    // Locate LIGHT 0:
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    // Enable lighting:
    glEnable(GL_LIGHTING);
    // Enable LIGHT 0:
    glEnable(GL_LIGHT0);
    
    glEnable(GL_DEPTH_TEST);                                        // Enable check for close and far objects.
    glClearColor(0.0, 0.0, 0.0, 0.0);                                    // Clear the color state.
    glMatrixMode(GL_MODELVIEW);                                        // Go to 3D mode.
    glLoadIdentity();                                            // Reset 3D view matrix.
}


void axes(float scale)
{
    glLineWidth(3);
    glBegin(GL_LINES);
    {
        glColor3f(1, 0, 0); // red
        glVertex3f(0, 0, 0);
        glVertex3f(scale, 0, 0);
        
        glColor3f(0, 1, 0); // green
        glVertex3f(0, 0, 0);
        glVertex3f(0, scale, 0);
        
        glColor3f(0, 0, 1); // blue
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, scale);
    }
    glEnd();
}

void display()                                                    // Called for each frame (about 60 times per second).
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                            // Clear color and depth buffers.
    glLoadIdentity();                                            // Reset 3D view matrix.
    gluLookAt(0.0, 0.0, -40.0,                                        // Where the camera is.
              0.0, 0.0, 0.0,                                        // To where the camera points at.
              0.0, 1.0, 0.0);                                        // "UP" vector.
    
    glutWireCube(20);
    axes(1);
    for (int p = 0; p < NUM_P; p++) {
        particles[p] -> draw();
    }
    glutSwapBuffers();                                            // Swap the hidden and visible buffers.
}

void idle()                                                    // Called when drawing is finished.
{
    if(gravityOn == 1){
        for (int p = 0; p < NUM_P; p++) {
            float gForce[3] = {0, -9.81f * particles[p]->mass, 0};
            
            if (particles[p]->pos[1] <= -10) {
                particles[p]->forces[1] = -particles[p]->forces[1] * particles[p]->restitutionCoefficient;
                particles[p]->pos[1] = -10;
                //gravityOn = 0;
                
                printf("Tocó el piso\n");
            }else{
                // right
                if (particles[p]->pos[0] >= 10) {
                    particles[p]->forces[0] = -particles[p]->forces[0] * particles[p]->restitutionCoefficient;
                    particles[p]->pos[0] = 10;
                    //gravityOn = 0;
                    
                    printf("Tocó la pared derecha\n");
                }else{
                    // back
                    if (particles[p]->pos[2] <= -10) {
                        particles[p]->forces[2] = -particles[p]->forces[2] * particles[p]->restitutionCoefficient;
                        particles[p]->pos[2] = -10;
                        //gravityOn = 0;
                        
                        printf("Tocó la pared trasera\n");
                    }else{
                        // left
                        if (particles[p]->pos[0] <= -10) {
                            particles[p]->forces[0] = -particles[p]->forces[0] * particles[p]->restitutionCoefficient;
                            particles[p]->pos[0] = -10;
                            //gravityOn = 0;
                            
                            printf("Tocó la pared izquierda\n");
                        }else{
                            // front
                            if (particles[p]->pos[2] >= 10) {
                                particles[p]->forces[2] = -particles[p]->forces[2] * particles[p]->restitutionCoefficient;
                                particles[p]->pos[2] = 10;
                                //gravityOn = 0;
                                
                                printf("Tocó la pared frontal\n");
                            }else{
                                // top
                                if (particles[p]->pos[1] >= 10) {
                                    particles[p]->forces[1] = -particles[p]->forces[1] * particles[p]->restitutionCoefficient;
                                    particles[p]->pos[1] = 10;
                                    //gravityOn = 0;
                                    
                                    printf("Tocó el techo\n");
                                }else{
                                    particles[p] -> addForce(gForce);
                                    printf("Ningún if\n");
                                }
                            }
                        }
                    }
                }
            }
            particles[p] -> stepSimulation(1/60.0f);
            
        }
        for (int p = 0; p < NUM_P; p++) {
            for (int k = 0; k < NUM_P; k++) {
                if (k != p) {
                    bool check = particles[p]->inCollision(particles[k]);
                    if (check == true) {
                        particles[p]->diffuse[0] = 1;
                        particles[p]->diffuse[1] = 0;
                        particles[p]->diffuse[2] = 0;
                    }
                }
            }
        }
    }else{
        for (int p = 0; p < NUM_P; p++) {
            // floor
            if (particles[p]->pos[1] <= -10) {
                particles[p]->forces[1] = -particles[p]->forces[1] * particles[p]->restitutionCoefficient;
                particles[p]->pos[1] = -10;
                gravityOn = 1;
                
                printf("Tocó el piso\n");
            }else{
                // right
                if (particles[p]->pos[0] >= 10) {
                    particles[p]->forces[0] = -particles[p]->forces[0] * particles[p]->restitutionCoefficient;
                    particles[p]->pos[0] = 10;
                    gravityOn = 1;
                    
                    printf("Tocó la pared derecha\n");
                }else{
                    // back
                    if (particles[p]->pos[2] <= -10) {
                        particles[p]->forces[2] = -particles[p]->forces[2] * particles[p]->restitutionCoefficient;
                        particles[p]->pos[2] = -10;
                        gravityOn = 1;
                        
                        printf("Tocó la pared trasera\n");
                    }else{
                        // left
                        if (particles[p]->pos[0] <= -10) {
                            particles[p]->forces[0] = -particles[p]->forces[0] * particles[p]->restitutionCoefficient;
                            particles[p]->pos[0] = -10;
                            gravityOn = 1;
                            
                            printf("Tocó la pared izquierda\n");
                        }else{
                            // front
                            if (particles[p]->pos[2] >= 10) {
                                particles[p]->forces[2] = -particles[p]->forces[2] * particles[p]->restitutionCoefficient;
                                particles[p]->pos[2] = 10;
                                gravityOn = 1;
                                
                                printf("Tocó la pared frontal\n");
                            }else{
                                // top
                                if (particles[p]->pos[1] >= 10) {
                                    particles[p]->forces[1] = -particles[p]->forces[1] * particles[p]->restitutionCoefficient;
                                    particles[p]->pos[1] = 10;
                                    gravityOn = 1;
                                    
                                    printf("Tocó el techo\n");
                                }else{
                                    particles[p] -> addForce(forces[p]);
                                    printf("Ningún if\n");
                                }
                            }
                        }
                    }
                }
            }
            
            particles[p] -> stepSimulation(1/60.0f);
        }
        for (int p = 0; p < NUM_P; p++) {
            for (int k = 0; k < NUM_P; k++) {
                if (k != p) {
                    bool check = particles[p]->inCollision(particles[k]);
                    if (check == true) {
                        particles[p]->diffuse[0] = 1;
                        particles[p]->diffuse[1] = 0;
                        particles[p]->diffuse[2] = 0;
                    }
                }
            }
        }
    }
    
    //    float gForce[3] = {0, -9.81f * p->mass, 0};
    //    p -> addForce(gForce);
    //    p -> stepSimulation(1/60.0f);
    glutPostRedisplay();                                            // Display again.
}

void reshape(int x, int y)                                            // Called when the window geometry changes.
{
    glMatrixMode(GL_PROJECTION);                                        // Go to 2D mode.
    glLoadIdentity();                                            // Reset the 2D matrix.
    gluPerspective(40.0, (GLdouble)x / (GLdouble)y, 0.1, 200.0);                        // Configure the camera lens aperture.
    glMatrixMode(GL_MODELVIEW);                                        // Go to 3D mode.
    glViewport(0, 0, x, y);                                            // Configure the camera frame dimensions.
    gluLookAt(0.0, 0.0, -40.0,                                        // Where the camera is.
              0.0, 0.0, 0.0,                                        // To where the camera points at.
              0.0, 1.0, 0.0);                                        // "UP" vector.
    display();
}

void keyboard(unsigned char key, int x, int y)
{
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    glutInit(&argc, argv);                                            // Init GLUT with command line parameters.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);                        // Use 2 buffers (hidden and visible). Use the depth buffer. Use 3 color channels.
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG Lighting");
    
    init();
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);                                        // Reshape CALLBACK function.
    glutDisplayFunc(display);                                        // Display CALLBACK function.
    glutIdleFunc(idle);                                            // Idle CALLBACK function.
    glutMainLoop();                                                // Begin graphics program.
    return 0;                                                // ANSI C requires a return value.
}

