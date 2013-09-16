#ifndef _DRAWING_H_
#define _DRAWING_H_

namespace cs354 {
    class Model;
    class Shader;
}

/* All variables / contants declared in this file are global. */

/*
 * Different drawing modes.  The drawing modes should begin at 0.
 * and should be consecutive integers.
 * 
 * DM_MAX should be the total number of drawing modes.
 */
enum DrawMode {
    DM_CUBE_GLUT        = 0,
    DM_CUBE_QUAD        = 1,
    DM_CUBE_QUAD_ARRAYS = 2,
    DM_CONE_GLUT        = 3,
    DM_CONE_TRI         = 4,
    DM_CONE_TRI_ARRAYS  = 5,
    DM_CONE_TRI_CALC    = 6,
    DM_VRML             = 7,
    DM_FREE_SCENE       = 8,
    
    DM_MAX
};
/*
#define DM_CUBE_GLUT            0
#define DM_CUBE_QUAD            1
#define DM_CUBE_QUAD_ARRAYS     2
#define DM_CONE_GLUT            3
#define DM_CONE_TRI             4
#define DM_CONE_TRI_ARRAYS      5
#define DM_CONE_TRI_CALC        6
#define DM_VRML                 7
#define DM_FREE_SCENE           8
#define DM_MAX                  9
*/

/* The current display mode */
extern int disp_mode;
extern cs354::Shader *shader;
extern cs354::Model *model;

/* Styles of drawing glut objects, either solid or wire-frame */
enum DrawStyle {
    DS_SOLID = 0,
    DS_WIRE  = 1
};
//#define DS_SOLID        0
//#define DS_WIRE         1

/* The current display style */
extern int disp_style;

/* Function Declarations */
void myInit(int argc, char **argv);
void myDisplay();
void myReshape(int width, int height);
void myKeyHandler(unsigned char ch, int x, int y);
void resetCamera(void);
int endCanvas(int status);
void performanceTest();
void initLighting();

void draw_cube_glut();
void draw_cube_quad();
void draw_cube_quad_arrays();
void draw_cone_glut();
void draw_cone_tri();
void draw_cone_tri_arrays();
void draw_cone_tri_calc(double height, double radius, int base_tri);
void draw_vrml();
void draw_free_scene();
void print_disp_mode();
void print_disp_style();

#endif	/* _DRAWING_H_ */
