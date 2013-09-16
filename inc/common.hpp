#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef _WIN32
# include <windows.h>
#endif

#ifdef __MAC__
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#else
# define GL_GLEXT_PROTOTYPES
# include <GL/gl.h>
# include <GL/glut.h>
# include <GL/glext.h>
#endif

enum Axis {
    X_AXIS,
    Y_AXIS,
    Z_AXIS
};

#endif
