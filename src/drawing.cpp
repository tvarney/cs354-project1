/*
 * drawing.c
 * ---------
 * Drawing subroutines for each of the various display modes in the canvas.
 * Also contains the quadrilateral information for a cube and the
 * triangulation information for a cone.
 *
 * Starter code for Project 1.
 *
 * Group Members: Troy Varney - tav285 [troy.a.varney@gmail.com]
 */

#include "common.hpp"

#include <stdio.h>
#include <cmath>

#include "drawing.hpp"
#include "vrml.hpp"
#include "generic/Model.hpp"
#include "generic/Shader.hpp"

#define PI_2 6.28318530718

/* The current display mode */
int disp_mode;

/* The current display style */
int disp_style;

cs354::Shader *shader = NULL;
cs354::Model *model = NULL;
bool draw_model;

/***********************************************************
 * Begin Cube Data
 ***********************************************************/
/*
 * Vertices used in the manually rendered cube.  This is cube
 * that is 1 x 1 x 1 centered at the origin.  Each group of
 * 3 numbers is a vertex, specified in Cartesian coordinates.
 */
GLfloat cube_vertices[] = {
    -0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
};

/*
 * The colors of each vertex in the low level cube.  The index
 * into this array corresponds to the index into cube_vert.
 */
GLfloat cube_colors[] = {
    0.5f, 0.5f, 0.5f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
};

/*
 * Indices into cube_vert, groups of 4 can be used to draw quadrilaterals
 * which represents a face of the cube.  For instance, the first
 * quad means that vertices 0, 1, 3, 2 of the cube_vertices[]
 * are used, in that order, to form the first face of the cube.
 *
 * Note that all front facing quads are specified in a counterclockwise order
 * (that is, if you are looking at the front of a quad, the vertices will
 * appear in counterclockwise order).
 */
GLuint cube_indices[] = {
    0, 2, 3, 1,
    2, 6, 7, 3,
    7, 6, 4, 5,
    4, 0, 1, 5,
    1, 3, 7, 5,
    0, 4, 6, 2,
};
/***********************************************************
 * End Cube Data
 ***********************************************************/


/***********************************************************
 * Begin Cone Data
 ***********************************************************/

/*
 * Data for the triangulation of the surface of a cone that is one
 * unit tall has a unit circle for its base.  The base is composed
 * of 8 equally sized triangles, and the lateral surface of the cone
 * is composed of a different set of 8 equally sized triangles.
 *
 * See documentation in the Cube Data section for information on
 * the meaning of the data in each array.
 */

GLfloat cone_vertices[] = {
    1.0, -0.5, 0.0,         /* begin unit circle at y = -0.5 */
    0.707, -0.5, 0.707,
    0.0, -0.5, 1.0,
    -0.707, -0.5, 0.707,
    -1.0, -0.5, 0.0,
    -0.707, -0.5, -0.707,
    0.0, -0.5, -1.0,
    0.707, -0.5, -0.707,    /* end unit circle at y = -0.5 */
    0.0, -0.5, 0.0,         /* center of the base */
    0.0, 0.5, 0.0,          /* top of the cone */
};

GLfloat cone_colors[] = {
    0.0f, 0.0f, 0.5f,
    0.0f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
};

/*
 * Each set of 3 indices represent the vertices of a triangle.
 * Each triangle faces to the outside of the cone.  The vertices
 * of these forward facing triangles are specified in
 * counterclockwise order.
 */
GLuint cone_indices[] = {
    0, 1, 8,
    1, 2, 8,
    2, 3, 8,
    3, 4, 8,
    4, 5, 8,
    5, 6, 8,
    6, 7, 8,
    7, 0, 8,
    1, 0, 9,
    2, 1, 9,
    3, 2, 9,
    4, 3, 9,
    5, 4, 9,
    6, 5, 9,
    7, 6, 9,
    0, 7, 9,
};
/***********************************************************
 * End Cone Data
 ***********************************************************/


/* Uses glut to draw a cube */
void draw_cube_glut(void) {
    /* Draw the cube using glut */

    glColor3f(1.0f, 0.0f, 0.0f);
    if (disp_style == DS_SOLID) {
        glutSolidCube(1.0f);
    } else if (disp_style == DS_WIRE) {
        glutWireCube(1.0f);
    }
}

/*
 * Draws a cube using the data arrays at the top of this file.
 * Iteratively draws each quad in the cube.
 */
void draw_cube_quad(void) {
    int num_indices;
    int i;
    int index1, index2, index3, index4;

    num_indices = sizeof(cube_indices) / sizeof(GLuint);

    /*
     * Loop over all quads that need to be drawn.
     * Step i by 4 because there are 4 vertices per quad.
     */
    for (i = 0; i < num_indices; i += 4) {
        /*
         * Find the index into the vertex array.  The value
         * in the cube_indices array refers to the index
         * of the ordered triples, not the index for the
         * actual GLfloats that comprise the cube_vertices array.
         * Thus, we need to multiple by 3 to get the real index.
         */
        index1 = cube_indices[i] * 3;
        index2 = cube_indices[i+1] * 3;
        index3 = cube_indices[i+2] * 3;
        index4 = cube_indices[i+3] * 3;
        
        glBegin(GL_QUADS); {
            glColor3fv(  &(cube_colors[index1]) );
            glVertex3fv( &(cube_vertices[index1]) );
            glColor3fv(  &(cube_colors[index2]) );
            glVertex3fv( &(cube_vertices[index2]) );
            glColor3fv(  &(cube_colors[index3]) );
            glVertex3fv( &(cube_vertices[index3]) );
            glColor3fv(  &(cube_colors[index4]) );
            glVertex3fv( &(cube_vertices[index4]) );
        } glEnd();
    }
}

/*
 * Draws a cube using the data arrays at the top of this file.
 * Uses GL's vertex arrays, index arrays, color arrays, etc.
 */
void draw_cube_quad_arrays(void) {
    int num_indices;

    num_indices = sizeof(cube_indices) / sizeof(GLuint);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
    glColorPointer(3, GL_FLOAT, 0, cube_colors);
    glDrawElements(GL_QUADS, num_indices, GL_UNSIGNED_INT, cube_indices);
    
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

/*
 * Uses glut to draw a cone.  Must render in either solid and wire
 * frame modes, based on the value of the variable disp_style.
 */
void draw_cone_glut(void) {
    /* ADD YOUR CODE HERE */
    glColor3f(0.0f, 0.0f, 1.0f);
    if (disp_style == DS_SOLID) {
        glutSolidCone(1.0f, 1.0f, 50, 50);
    } else if (disp_style == DS_WIRE) {
        glutWireCone(1.0f, 1.0f, 50, 50);
    }
}

/*
 * Draws a cone using the data arrays at the top of this file.
 * Iteratively draws each triangle in the cone.
 */
void draw_cone_tri(void) {
    int num_indices = sizeof(cone_indices) / sizeof(GLuint);

    int index1, index2, index3;
    for(int i = 0; i < num_indices; i += 3) {
        index1 = cone_indices[i] * 3;
        index2 = cone_indices[i+1] * 3;
        index3 = cone_indices[i+2] * 3;
        
        glBegin(GL_TRIANGLES); {
            glColor3fv( &(cone_colors[index1]));
            glVertex3fv(&(cone_vertices[index1]));
            glColor3fv( &(cone_colors[index2]));
            glVertex3fv(&(cone_vertices[index2]));
            glColor3fv( &(cone_colors[index3]));
            glVertex3fv(&(cone_vertices[index3]));
        } glEnd();
    }
}

/*
 * Draws a cone using the data arrays at the top of this file.
 * Uses GL's vertex arrays, index arrays, color arrays, etc.
 */
void draw_cone_tri_arrays(void) {
    int num_indices = sizeof(cone_indices) / sizeof(GLuint);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, 0, cone_vertices);
    glColorPointer(3, GL_FLOAT, 0, cone_colors);
    glDrawElements(GL_QUADS, num_indices, GL_UNSIGNED_INT, cone_indices);
    
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

/*
 * Draws a cone using a calculated triangulation of the cone surface.
 *
 * Args
 * ----
 * The HEIGHT and RADIUS of the cone.
 *
 * BASE_TRI refers to the number of triangles used to represent
 * the base of the cone.  Each of these triangles should share a
 * common vertex, namely, the center of the base.
 *
 * The final triangulation of the cone surface should include
 * exactly 2 * BASE_TRI.
 */
void draw_cone_tri_calc(double height, double radius, int base_tri) {
    /* Set up our rendering mode */
    double rad_frac = 1.0 / double(base_tri) * PI_2;
    double currRad = 0.0;
    GLfloat x1, z1, x2, z2;
    
    x1 = radius; /*< cos(0.0) = 1.0 */
    z1 = 0.0; /*< sin(0.0) = 0.0 */
    
    glColor3f(0.0, 0.0, 1.0);
    if(disp_style == DS_WIRE) {
        glBegin(GL_LINES); {
            for(int i = 0; i < base_tri; i++) {
                currRad += rad_frac;
                x2 = cos(currRad) * radius;
                z2 = sin(currRad) * radius;
                
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(x1, 0.0, z1);
                
                glVertex3f(x1, 0.0, z1);
                glVertex3f(x2, 0.0, z2);
                
                glVertex3f(x1, 0.0, z1);
                glVertex3f(0.0, height, 0.0);
                
                x1 = x2;
                z1 = z2;
            }
        } glEnd();
    }else {
        glBegin(GL_TRIANGLES); {
            for(int i = 0; i < base_tri; i++) {
                currRad += rad_frac;
                x2 = cos(currRad) * radius;
                z2 = sin(currRad) * radius;
                
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(x2, 0.0, z2);
                glVertex3f(x1, 0.0, z1);
                
                glVertex3f(0.0, height, 0.0);
                glVertex3f(x2, 0.0, z2);
                glVertex3f(x1, 0.0, z1);
                
                x1 = x2;
                z1 = z2;
            }
        } glEnd();
    }
}

/* Draw the various vrml scenes */
void draw_vrml(void) {
    /* ADD YOUR CODE HERE */
    /* NOTE: you should be calling a function or functions in vrml.c */
    GLenum mode = GL_POLYGON;
    if (disp_style == DS_SOLID) {
        mode = GL_POLYGON;
    } else if (disp_style == DS_WIRE) {
        mode = GL_LINE_LOOP;
    }
    vrml::CurrentObject().draw(mode);
}

/* Draws a freeform scene */

cs354::Material _torus_mat = {
    {0.5, 0.0, 0.0},
    {0.75, 0.0, 0.0},
    {0.8, 0.2, 0.2},
    1.0, 50.0,
    1, 0,0,0,0,0,0
};
cs354::Material _sphere_mat = {
    {0.0, 0.5, 0.0},
    {0.1, 0.75, 0.1},
    {0.2, 0.8, 0.2},
    1.0, 50.0,
    1, 0,0,0,0, 0,0
};
void draw_free_scene(void) {
    if(shader != NULL) {
        shader->use();
        GLuint prog = shader->handle();
        GLint loc = glGetUniformLocation(prog, "Light.Position");
        glUniform4f(loc, 2.0, 10.0, -2.0, 0.0);
        loc = glGetUniformLocation(prog, "Light.La");
        glUniform3f(loc, 1.0, 1.0, 1.0);
        loc = glGetUniformLocation(prog, "Light.Ld");
        glUniform3f(loc, 1.0, 1.0, 1.0);
        loc = glGetUniformLocation(prog, "Light.Ls");
        glUniform3f(loc, 1.0, 1.0, 1.0);
    }
    
    if(model != NULL && draw_model) {
        model->draw();
    }else {
        if(shader != NULL) {
            _torus_mat.bind();
        }else {
            glColor3f(0.5, 0.0, 0.0);
        }
        glutSolidTorus(0.1, 0.4, 100, 40);
        
        glPushMatrix();
        glTranslatef(1.0f, 0.0f, 1.0f);
        if(shader != NULL) {
            _sphere_mat.bind();
        }else {
            glColor3f(0.0f, 0.5f, 0.0f);
        }
        glutSolidSphere(1.0f, 100, 100);
        glPopMatrix();
    }
    if(shader) {
        cs354::Shader::UseDefaultShaders();
    }
}


/* Prints to stdout the current display mode */
static const char * _disp_modes[] = {
    "Cube using glut",
    "Cube using quadrilaterals",
    "Cube using quadrilateral arrays",
    "Cone using glut",
    "Cone using triangles",
    "Cone using triangle arrays",
    "Cone using calculated triangles",
    "VRML objects",
    "Freeform scene"
};
void print_disp_mode( void ) {
    if(disp_mode < 0 || disp_mode > 8) {
        printf("Warning: unknown display mode\n");        
    }else {
        printf("Display Mode: %s\n", _disp_modes[disp_mode]);
    }
}

/* Prints to stdout the current display style */
static const char * _disp_styles[] = {
    "solid (for glut modes only)",
    "wire (for glut modes only)"
};
void print_disp_style( void ) {
    if(disp_style < 0 || disp_style > 1) {
        printf("Warning: unknown display style\n");
    }else {
        printf("Display Style: %s\n", _disp_styles[disp_style]);
    }
}

/* end of drawing.c */

