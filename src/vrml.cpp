/*
 * vrml.c
 * ------
 * Drawing subroutines for each vrml object.
 * Also contains the polygon data for each vrml object.
 *
 * Starter code for Project 1.
 *
 * Group Members: Troy Varney - tav285 [troy.a.varney@gmail.com]
 */

#include "common.hpp"
#include "vrml.hpp"

#include <stdio.h>
#include <stdlib.h>

using namespace vrml;

/*
 * Note that in VRML, the face indices have a -1 to represent
 * the termination of a index sequence.
 */
Object::Object(const char *name, int nv, GLfloat *vertices, int nf,
               GLint *faces) :
    name(name), nVertices(nv), nFaces(nf), vertices(vertices), indices(faces)
{ }
Object::~Object() { }

void Object::draw(GLenum mode) {
    /* Sanity check */
    if(mode != GL_POLYGON && mode != GL_LINE_LOOP) {
        mode = GL_POLYGON;
    }
    
    glColor3f(1.0, 1.0, 0.0);
    int offset = 0, vOff;
    for(int i = 0; i < nFaces; ++i) {
        glBegin(mode); {
            while(indices[offset] != -1) {
                vOff = indices[offset] * 3;
                glVertex3fv(&(vertices[vOff]));
                offset += 1;
            }
            offset += 1;
        } glEnd();
    }
}

/***********************************************************
 * Begin VRML Cube Data
 ***********************************************************/
static GLfloat cube_vertices[] = {
    1.632990, 0.000000, 1.154700,
    0.000000, 1.632990, 1.154700,
    -1.632990, 0.000000, 1.154700,
    0.000000, -1.632990, 1.154700,
    0.000000, -1.632990, -1.154700,
    1.632990, 0.000000, -1.154700,
    0.000000, 1.632990, -1.154700,
    -1.632990, 0.000000, -1.154700,
};
static GLint cube_indices[] = {
    0, 1, 2, 3, -1,
    4, 5, 0, 3, -1,
    5, 6, 1, 0, -1,
    6, 7, 2, 1, -1,
    3, 2, 7, 4, -1,
    7, 6, 5, 4, -1,
};
static Object cube("Cube", 8, cube_vertices, 6, cube_indices);

/***********************************************************
 * Begin VRML Dodecahedron Data
 ***********************************************************/
static GLfloat dodeca_vertices[] = {
    1.214120, 0.000000, 1.589310,
    0.375185, 1.154700, 1.589310,
    -0.982247, 0.713644, 1.589310,
    -0.982247, -0.713644, 1.589310,
    0.375185, -1.154700, 1.589310,
    1.964490, 0.000000, 0.375185,
    1.589310, 1.154700, -0.375185,
    0.607062, 1.868350, 0.375185,
    -0.607062, 1.868350, -0.375185,
    -1.589310, 1.154700, 0.375185,
    -1.964490, 0.000000, -0.375185,
    -1.589310, -1.154700, 0.375185,
    -0.607062, -1.868350, -0.375185,
    0.607062, -1.868350, 0.375185,
    1.589310, -1.154700, -0.375185,
    0.982247, 0.713644, -1.589310,
    0.982247, -0.713644, -1.589310,
    -0.375185, 1.154700, -1.589310,
    -1.214120, 0.000000, -1.589310,
    -0.375185, -1.154700, -1.589310,
};
static GLint dodeca_indices[] = {
    0, 1, 2, 3, 4, -1,
    0, 5, 6, 7, 1, -1,
    1, 7, 8, 9, 2, -1,
    2, 9, 10, 11, 3, -1,
    3, 11, 12, 13, 4, -1,
    4, 13, 14, 5, 0, -1,
    15, 6, 5, 14, 16, -1,
    17, 8, 7, 6, 15, -1,
    18, 10, 9, 8, 17, -1,
    19, 12, 11, 10, 18, -1,
    16, 14, 13, 12, 19, -1,
    16, 19, 18, 17, 15, -1,
};
static Object dodeca("Dodecahedron", 20, dodeca_vertices, 12, dodeca_indices);

/***********************************************************
 * Begin VRML Icosahedron Data
 ***********************************************************/
static GLfloat icosa_vertices[] = {
    0.552786, 1.701300, 0.894427,
    0.000000, 0.000000, 2.000000,
    1.788850, 0.000000, 0.894427,
    -1.447210, 1.051460, 0.894427,
    -1.447210, -1.051460, 0.894427,
    0.552786, -1.701300, 0.894427,
    1.447210, 1.051460, -0.894427,
    -0.552786, 1.701300, -0.894427,
    -1.788850, 0.000000, -0.894427,
    -0.552786, -1.701300, -0.894427,
    1.447210, -1.051460, -0.894427,
    0.000000, 0.000000, -2.000000,
};
static GLint icosa_indices[] = {
    0, 1, 2, -1,
    3, 1, 0, -1,
    4, 1, 3, -1,
    5, 1, 4, -1,
    2, 1, 5, -1,
    0, 2, 6, -1,
    7, 0, 6, -1,
    3, 0, 7, -1,
    8, 3, 7, -1,
    4, 3, 8, -1,
    9, 4, 8, -1,
    5, 4, 9, -1,
    10, 5, 9, -1,
    6, 2, 10, -1,
    2, 5, 10, -1,
    6, 11, 7, -1,
    7, 11, 8, -1,
    8, 11, 9, -1,
    9, 11, 10, -1,
    10, 11, 6, -1,
};
static Object icosa("Icosahedron", 12, icosa_vertices, 20, icosa_indices);

/***********************************************************
 * Begin VRML Pyramid Data
 ***********************************************************/
static GLfloat pyramid_vertices[] = {
    0.997029, 0.000000, -0.997029,
    0.012175, 1.000000, -0.012175,
    -0.997029, 0.000000, -0.997029,
    -0.012175, 1.000000, -0.012175,
    -0.997029, 0.000000, 0.997029,
    -0.012175, 1.000000, 0.012175,
    0.997029, 0.000000, 0.997029,
    0.012175, 1.000000, 0.012175,
};
static GLint pyramid_indices[] = {
    6, 0, 7, -1,
    7, 0, 1, -1,
    0, 2, 1, -1,
    1, 2, 3, -1,
    2, 4, 3, -1,
    3, 4, 5, -1,
    4, 6, 5, -1,
    5, 6, 7, -1,
    4, 0, 6, -1,
    4, 2, 0, -1,
    7, 1, 5, -1,
    1, 3, 5, -1,
};
static Object pyramid("Pyramid", 8, pyramid_vertices, 12, pyramid_indices);

static Object *objarray[] = { &cube, &dodeca, &icosa, &pyramid };

static int nobjects = 4;
static int currobj = 0;
Object & vrml::CurrentObject() {
    return *(objarray[currobj]);
}
void vrml::NextObject() {
    currobj = (currobj + 1) % nobjects;
}
void vrml::PrevObject() {
    currobj -= 1;
    if(currobj < 0) {
        currobj = nobjects - 1;
    }
}
void vrml::SetObject(int num) {
    if(num >= nobjects) {
        currobj = nobjects - 1;
    }else if(num < 0) {
        currobj = 0;
    }else {
        currobj = num;
    }
}

int vrml::NumObjects() {
    return nobjects;
}
