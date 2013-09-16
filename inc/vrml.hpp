#ifndef _VRML_H_
#define _VRML_H_

#include "common.hpp"

/* The current vermal object */
namespace vrml {
    struct Object {
        Object(const char *name, int nv, GLfloat *vertices, int nf,
               GLint *faces);
        ~Object();
        
        void draw(GLenum mode);
        const char *name;
        int nVertices, nFaces;
        GLfloat *vertices;
        GLint *indices;
    };
    
    Object & CurrentObject();
    void NextObject();
    void PrevObject();
    void SetObject(int num);
    int NumObjects();
}

#endif	/* _VRML_H_ */

