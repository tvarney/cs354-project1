/*
 * canvas.c
 * --------
 * Implements a canvas with multiple display modes.
 * Starter code for Project 1.
 *
 * Group Members: Troy Varney - tav285 [troy.a.varney@gmail.com]
 */

#include "common.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>

#include "drawing.hpp"
#include "vrml.hpp"
#include "mouse.hpp"
#include "generic/Model.hpp"
#include "generic/ModelIO.hpp"
#include "generic/Shader.hpp"

/* The current vrml object */
int vr_object;

/* The canvas's width and height, in pixels */
int win_width = 500;
int win_height = 500;

/* The dimensions of the viewing frustum */
GLfloat fleft   = -1.0;
GLfloat fright  =  1.0;
GLfloat fbottom = -1.0;
GLfloat ftop    =  1.0;
GLfloat zNear   = -2.0;
GLfloat zFar    = -7.0;

/* Global zoom factor.  Modified by user input. Initially 1.0 */
GLfloat zoomFactor = 1.0; 

double _height = 1.0, _radius = 1.0, _base_tri = 8;

/*
 * Performs specific initializations for this program (as opposed to
 * glut initialization.
 */
static const char _vshader[] = "./data/shaders/free.vs";
static const char _fshader[] = "./data/shaders/free.fs";
static const char _model[] = "./data/model/model.obj";
void init(int argc, char **argv) {
    /* Set the default display mode and style */
    disp_mode = DM_CUBE_GLUT;
    disp_style = DS_SOLID;
    
    /* Set up a black background */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    resetCamera();
    
    if(shader) {
        fputs("Warning: Shader already initialized. This shouldn't happen",
              stderr);
    }else {
        puts("Loading shaders from ./data/shaders ...");
        try {
            shader = new cs354::Shader();
            FILE *fp = fopen(_vshader, "r");
            if(!fp) {
                fprintf(stderr, "Could not open '%s'\n", _vshader);
                delete shader;
                shader = NULL;
                return;
            }
            shader->add(GL_VERTEX_SHADER, fp);
            fclose(fp);
            
            fp = fopen(_fshader, "r");
            if(!fp) {
                fprintf(stderr, "Could not open '%s'\n", _fshader);
                delete shader;
                shader = NULL;
                return;
            }
            shader->add(GL_FRAGMENT_SHADER, fp);
            
            shader->link();
        }catch(std::exception &err) {
            fprintf(stderr, "Could not load shaders:\n%s\n", err.what());
        }catch(...) {
            fputs("Unknown error", stderr);
        }
    }
    if(model) {
        fputs("Warning: Model already initialized. This shouldn't happen.",
              stderr);
    }else {
        printf("Loading model from %s\n", _model);
        try {
            model = cs354::ModelIO::Load(_model);
        }catch(std::exception &err) {
            fprintf(stderr, "Could not load model:\n%s\n", err.what());
        }catch(...) {
            fputs("Unknown error", stderr);
        }
    }
}

/*
 * The main drawing routine.  Based on the current display mode, other
 * helper functions may be called.
 */
void myDisplay (void) {
    
    glEnable(GL_DEPTH_TEST);	/* Use the Z - buffer for visibility */
    glMatrixMode(GL_MODELVIEW);	/* All matrix operations are for the model */
    
    /* Clear the pixels (aka colors) and the z-buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Only use lighting if we aren't in free scene */
    switch (disp_mode) {
    case DM_CUBE_GLUT:
        draw_cube_glut();
        break;
    case DM_CUBE_QUAD:
        draw_cube_quad();
        break;
    case DM_CUBE_QUAD_ARRAYS:
        draw_cube_quad_arrays();
        break;
    case DM_CONE_GLUT:
        draw_cone_glut();
        break;
    case DM_CONE_TRI:
        draw_cone_tri();
        break;
    case DM_CONE_TRI_ARRAYS:
        draw_cone_tri_arrays();
        break;
    case DM_CONE_TRI_CALC:
        draw_cone_tri_calc(_radius, _height, _base_tri); /*< HERE */
        break;
    case DM_VRML:
        draw_vrml();
        break;
    case DM_FREE_SCENE:
        draw_free_scene();
        break;
    default:
        printf("myDisplay Warning: unrecognized Display Mode\n");
        break;
    }
    
    glFlush();	/* Flush all executed OpenGL ops finish */
    
    /*
     * Since we are using double buffers, we need to call the swap
     * function every time we are done drawing.
     */
    glutSwapBuffers();
}


/*
 * Changes the size of the canvas's window, and will implicitly
 * the function bound by glutReshapeFunc(), which should be
 * Reshape().
 */
void myResize (int x, int y) {
    glViewport(0,0,x,y);
    glutReshapeWindow(x, y);
}


/* Stretch the image to fit the reshaped window */
void myReshape (int x, int y) {
    glViewport(0,0,x,y);
}


/*
 * The rotation is specified in degrees about a certain axis of
 * the original model.
 *
 * AXIS should be either X_AXIS, Y_AXIS, or Z_AXIS.
 *
 * Positive degrees rotate in the counterclockwise direction.
 */
void rotateCamera(double deg, Axis axis) {
    double x, y, z;
    
    x = 0;
    y = 0;
    z = 0;
    
    switch(axis) {
    case X_AXIS:
        x = 1.0f;
        break;
    case Y_AXIS:
        y = 1.0f;
        break;
    case Z_AXIS:
        z = 1.0f;
        break;
    default:
        return;
    }
    
    glRotatef(deg, x, y, z);
}

/*
 * Changes the level of zooming by adjusting the dimenstions of the viewing
 * frustum.
 *
 * Args: delta - the change in the zoom factor.  Negative deltas cause the
 * camera to zoom in, while positive values cause the camera to zoom out.
 */
void zoomCamera(double delta) {
    zoomFactor += delta;
    
    if (zoomFactor <= 0.0) {
        /* The zoom factor should be positive */
        zoomFactor = 0.001;
    }
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    /*
     * glFrustum must receive positive values for the near and far clip planes
     * ( arguments 5 and 6 ).
     */
    glFrustum(fleft*zoomFactor, fright*zoomFactor,
              fbottom*zoomFactor, ftop*zoomFactor,
              -zNear, -zFar);
}

/*
 * Resets the viewing frustum and moves the drawing point to the center of
 * the frustum.
 */
void resetCamera( void ) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    /*
     * glFrustum must receive positive values for the near and far clip planes
     * ( arguments 5 and 6 ).
     */
    glFrustum(fleft, fright, fbottom, ftop, -zNear, -zFar);
    
    /* Set the drawing point at the center of the frustum */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /* Go to the center of the scene */
    glTranslatef((fleft + fright)/2, (fbottom + ftop)/2, (zNear + zFar)/2);
    
    zoomFactor = 1.0;
}

void performanceTest(void) {
    int start, end;
    int i;
    int curr_width, curr_height;
    
    /* Give a warning if the window has been resized */
    curr_width = glutGet(GLUT_WINDOW_WIDTH);
    curr_height = glutGet(GLUT_WINDOW_HEIGHT);
    
    if ((curr_width != win_width) || (curr_height != win_height)) {
        printf("*** Warning ***\n");
        printf("The window has been resized and results may be inaccurate.\n");
        printf("First press 'z' to restore the default window size.\n");
        printf("*** Warning ***\n");
    }
    
    resetCamera();
    
    printf("Initiating Performance Test\n");
    start = glutGet(GLUT_ELAPSED_TIME);
    
    /* For every rotation, the display loop will be recalled */
    for (i = 0; i < 360; i++) {
        rotateCamera(1.0, X_AXIS);
        myDisplay();		/* refresh the screen */
    }
    for (i = 0; i < 360; i++) {
        rotateCamera(1.0, Y_AXIS);
        myDisplay();		/* refresh the screen */
    }
    for (i = 0; i < 360; i++) {
        rotateCamera(1.0, Z_AXIS);
        myDisplay();		/* refresh the screen */
    }
    
    end = glutGet(GLUT_ELAPSED_TIME);
    
    /* Return the number of milliseconds elapsed */
    printf("Performance Test completed in %.2f sec\n",
           (end - start) / 1000.0f);
}

/* Handle user input */
void myKeyHandler(unsigned char ch, int x, int y) {
    switch(ch) {
    case 'm':
    case 'M':
        draw_model = !draw_model;
        break;
    case 'I':
        _radius += 0.1;
        break;
    case 'i':
        _radius -= 0.1;
        if(_radius < 0.1) {
            _radius = 0.1;
        }
        break;
    case 'O':
        _height += 0.1;
        break;
    case 'o':
        _height -= 0.1;
        if(_height < 0.1) {
            _height = 0.1;
        }
        break;
    case 'P':
        _base_tri += 1;
        break;
    case 'p':
        _base_tri -= 1;
        if(_base_tri < 3) {
            _base_tri = 3;
        }
        break;
    case 'v':
        vrml::NextObject();
        printf("Drawing VRML %s\n", vrml::CurrentObject().name);
        break;
    case 'c':
        resetCamera();
        printf("Camera reset.\n");
        break;
    case 's':
    case 'S':
        if (disp_style == DS_SOLID) {
            disp_style = DS_WIRE;
        } else {
            disp_style = DS_SOLID;
        }
        print_disp_style();
        break;
    case 'd':
        /* Cycle through the various display modes */
        disp_mode = (disp_mode + 1) % DM_MAX;
        print_disp_mode();
        break;
    case 'D':
        /* Cycle through the various display modes backwards */
        /* By adding DM_MAX, the args to "%" wil never be negative */
        disp_mode = (disp_mode + DM_MAX - 1) % DM_MAX;
        print_disp_mode();
        break;
    case ',':
        rotateCamera(5, X_AXIS);
        break;
    case '<':
        rotateCamera(-5, X_AXIS);
        break;
    case '.':
        rotateCamera(5, Y_AXIS);
        break;
    case '>':
        rotateCamera(-5, Y_AXIS);
        break;
    case '/':
        rotateCamera(5, Z_AXIS);
        break;
    case '?':
        rotateCamera(-5, Z_AXIS);
        break;
    case '+':
        /* Zoom in */
        zoomCamera(-0.1);
        break;
    case '=':
        /* Zoom out */
        zoomCamera(0.1);
        break;
    case 'z':
        myResize(win_width, win_height);
        printf("Window set to default size.\n");
        break;
    case 't':
        performanceTest();
        break;
    case 'q':
        /* Quit with exit code 0 */
        endCanvas(0);
        break;
        /*********************************************/
        /* ADD ADDITIONAL KEYS HERE                  */
        /*********************************************/
        
        
    default:
        /* Unrecognized key press, just return */
        return;
    }
    
    /*
     * If control reaches here, the key press was recognized.  Refresh
     * the screen, since most key presses change the display in some way.
     */
    myDisplay();
    
    return;
}


int endCanvas(int status) {
    printf("\nQuitting canvas.\n\n");
    fflush(stdout);
    
    exit(status);
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    
    /* Set initial window size and screen offset */
    glutInitWindowSize(win_width, win_height);
    glutInitWindowPosition(50, 50);
    
    /* Using: RGB (no alpha), double buffering, z-buffer */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    
    glutCreateWindow("Canvas");
    
    /* Set the function callbacks */
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(myKeyHandler);
    glutMouseFunc(myMouseButton);
    glutMotionFunc(myMouseMotion);
    
    /* User specific initialization */
    init(argc, argv);
    /* Go into the main glut control loop, will not return */
    glutMainLoop();
    
    /* Control flow will never reach here */
    return 0;
}

/* end of canvas.c */
