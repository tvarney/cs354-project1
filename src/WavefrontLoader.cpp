
#include "generic/WavefrontLoader.hpp"
#include "generic/Model.hpp"

#include <climits>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

using namespace cs354;

/**************************************************/
/* Symbols from the bison generated parsers */
extern int wf_parse(void);
extern void wf_restart(FILE *fp);
extern int mat_parse(void);
extern void mat_restart(FILE *fp);
/**************************************************/

/**************************************************/
/* Defines to make my life easier */
#define RuntimeError(msg) std::runtime_error(std::string(msg))
#define COPY_TRIPLE(to, from) \
    (to)[0] = (from)[0];      \
    (to)[1] = (from)[1];      \
    (to)[2] = (from)[2]
/**************************************************/

/**************************************************/
Element::Element(int args[3]) :
    v(args[0]), vt(args[1]), vn(args[2])
{ }
Element::~Element() { }
/**************************************************/

/**************************************************/
Vertex::Vertex(GLfloat args[3]) :
    x(args[0]), y(args[1]), z(args[2])
{ }
Vertex::~Vertex() { }
/**************************************************/

/**************************************************/
TextureCoord::TextureCoord(GLfloat args[3]) :
    u(args[0]), v(args[1]), w(args[2])
{ }
TextureCoord::~TextureCoord() { }
/**************************************************/

/**************************************************/
Normal::Normal(GLfloat args[3]) :
    xn(args[0]), yn(args[1]), zn(args[2])
{ }
Normal::~Normal() { }
/**************************************************/

/**************************************************/
LoaderMatGroup::LoaderMatGroup() { }
LoaderMatGroup::LoaderMatGroup(const std::string &mtlname) :
    mtlname(mtlname)
{ }
LoaderMatGroup::~LoaderMatGroup() { }
/**************************************************/

/**************************************************/
LoaderGroup::LoaderGroup() { }
LoaderGroup::LoaderGroup(const std::string &name) :
    name(name)
{ }
LoaderGroup::~LoaderGroup() { }
/**************************************************/

/**************************************************/
LoaderObject::LoaderObject() { }
LoaderObject::LoaderObject(const std::string &name) :
    name(name)
{ }
LoaderObject::~LoaderObject() { }
/**************************************************/

/**************************************************/
WavefrontLoader::WavefrontLoader(bool keep_materials, bool global_mats) :
    logFile(stderr), keepMaterials(keep_materials),
    globalMaterials(global_mats)
{ }
WavefrontLoader::~WavefrontLoader() { }

Model * WavefrontLoader::load(const char *fname, FILE *fp) {
    Model *model = NULL;
    clear();
    
    next.hasMtl = next.hasGroup = next.hasObject = false;
    
    /* Open the file if we need to. 'openfile' is defined to allow us to close
     * the file when we are done if we need to (as in, if we opened the file
     * in this method).
     */
    bool openfile = (fp == NULL);
    if(openfile) {
        fp = fopen(fname, "r");
        if(!fp) {
            /* Couldn't open the file, throw an exception to abort. */
            throw RuntimeError("Could not open file");
        }
    }
    
    /* Call the parser; this may throw an exception due to the wf_parse method
     * calling methods of WavefrontLoader that throw exceptions.
     */
    wf_restart(fp);
    int rval = wf_parse();
    
    /* Examine the return value. 0 = good, 1 = Invalid Syntax,
     * 2 = Out of Memory
     */
    switch(rval) {
    case 0:
        break;
    case 1:
        throw RuntimeError("Invalid Syntax in wavefront .obj file.");
    case 2:
        throw RuntimeError("Parser exhausted memory.");
    default:
        throw RuntimeError("Unknown error in parser.");
        break;
    }
    
    /* Close the file since we are done with it. Here is why we saved openfile
     */
    if(openfile) {
        fclose(fp);
    }
    
    /* TODO: Convert cached values into a valid model. */
    
    clear();
    return model;
}

void WavefrontLoader::use(std::map<std::string, Material> & global_mat_map) {
    globalMaterialMap = &global_mat_map;
}

/**************************************************/
/* Parser interface */

void WavefrontLoader::v(GLfloat coords[3]) {
    vertices.push_back(Vertex(coords));
}
void WavefrontLoader::vn(GLfloat coords[3]) {
    normals.push_back(Normal(coords));
}
void WavefrontLoader::vt(GLfloat coords[3]) {
    texCoords.push_back(TextureCoord(coords));
}
void WavefrontLoader::f() {
    /* Resolve any outstanding object, group or material requests */
    if(current.object == NULL || next.hasObject) {
        newObject(next.hasObject ? next.object : std::string(""));
    }else if(current.group == NULL || next.hasGroup) {
        newGroup(next.hasGroup ? next.group : std::string(""));
    }else if(current.mgroup == NULL || next.hasMtl) {
        newMatGroup(next.hasMtl ? next.mtl : std::string(""));
    }
    
    size_t fs_size = faceStack.size();
    if(fs_size < 3) {
        log("Too few arguments to f: %d\n", int(fs_size));
        return;
    }
    
    resolve(faceStack[0]);
    resolve(faceStack[1]);
    resolve(faceStack[2]);
    Triangle t = { faceStack[0], faceStack[1], faceStack[2] };
    current.mgroup->faces.push_back(t);
    for(size_t i = 3; i < fs_size; ++i) {
        resolve(faceStack[i]);
        t.v2 = t.v3;
        t.v3 = faceStack[i];
        current.mgroup->faces.push_back(t);
    }
    
    if(fs_size > 3) {
        log("face tesselated into %llu triangles\n", fs_size - 2);
    }
}
void WavefrontLoader::fArg(int args[3]) {
    faceStack.push_back(Element(args));
}
void WavefrontLoader::mtllib(const char *libname) {
    /* Here is the fun part, we call a new parser from within another parser */
    
    
}
void WavefrontLoader::usemtl(const char *mtlname) {
    std::string name = mtlname;
    /* Check if the material requested has been loaded
     * Invalid references will still create material groups, they will just
     * use the default material.
     * "" is used to represent the default material, which skips the checks
     * for the material in the material maps.
     */
    std::map<std::string, Material>::iterator miter;
    if(std::strcmp(mtlname, "") != 0) {
        miter = materials.find(name);
        if(miter == materials.end()) {
            if(globalMaterialMap != NULL) {
                miter = (*globalMaterialMap).find(name);
                if(miter == (*globalMaterialMap).end()) {
                    log("Invalid material reference: %s\n", mtlname);
                }
            }else {
                log("Invalid material reference: %s\n", mtlname);
            }
        }
    }
    
    /* Check if the material group already exists */
    std::map<std::string, LoaderMatGroup>::iterator iter;
    iter = current.group->material_groups.find(name);
    if(iter != current.group->material_groups.end()) {
        /* Set current material group to existing group */
        current.mgroup = &(iter->second);
    }else {
        /* Create new group, set the pointer to it */
        current.group->material_groups[name] = LoaderMatGroup(name);
        current.mgroup = &(current.group->material_groups[name]);
    }
}
void WavefrontLoader::g(const char *groupname) {
    
}
void WavefrontLoader::o(const char *objectname) {
    current.group = NULL;
    current.mgroup = NULL;
}

/* Material commands */
void WavefrontLoader::newmtl(const char *mtlname) {
    
}
void WavefrontLoader::ka(GLfloat color[3]) {
    
}
void WavefrontLoader::kd(GLfloat color[3]) {
    
}
void WavefrontLoader::ks(GLfloat color[3]) {
    
}
void WavefrontLoader::ns(GLfloat amount) {
    
}
void WavefrontLoader::tr(GLfloat amount) {
    
}

/**************************************************/
/* Private methods of WavefrontLoader */

void WavefrontLoader::clear() {
    /* This should clean up each sub-map */
    objects.clear();
    faceStack.clear();
    vertices.clear();
    texCoords.clear();
    normals.clear();
    if(!keepMaterials) {
        materials.clear();
    }
    current.object = NULL;
    current.group = NULL;
    current.mgroup = NULL;
    matdef = Material::Default;
}

void WavefrontLoader::log(const char *msg, ...) {
    va_list vargs;
    
    va_start(vargs, msg);
    fputs("WavefrontLoader:: ", logFile);
    vfprintf(logFile, msg, vargs);
    va_end(vargs);
}

void WavefrontLoader::resolve(Element &e) {
    if(e.v < 0) {
        e.v = vertices.size() + e.v;
    }else if(e.v == 0) {
        e.v = -1;
    }else {
        e.v -= 1;
    }
    
    if(e.vt < 0) {
        e.vt = texCoords.size() + e.vt;
    }else if(e.vt == 0) {
        e.vt = -1;
    }else {
        e.vt -= 1;
    }
    
    if(e.vn < 0) {
        e.vn = normals.size() + e.vn;
    }else if(e.vn == 0) {
        e.vn = -1;
    }else {
        e.vn -= 1;
    }
}

void WavefrontLoader::newObject(const std::string &name) {
    std::map<std::string, LoaderObject>::iterator iter;
    
    iter = objects.find(name);
    if(iter == objects.end()) {
        objects[name] = LoaderObject(name);
        current.object = &(objects[name]);
    }else {
        current.object = &(iter->second);
    }
    
    newGroup(next.hasGroup ? next.group : std::string(""));
    next.hasObject = false;
}

void WavefrontLoader::newGroup(const std::string &name) {
    std::map<std::string, LoaderGroup>::iterator iter;
    
    iter = current.object->groups.find(name);
    if(iter == current.object->groups.end()) {
        current.object->groups[name] = LoaderGroup(name);
        current.group = &(current.object->groups[name]);
    }else {
        current.group = &(iter->second);
    }
    
    newMatGroup(next.hasMtl ? next.mtl : std::string(""));
    next.hasGroup = false;
}

void WavefrontLoader::newMatGroup(const std::string &name) {
    std::map<std::string, LoaderMatGroup>::iterator iter;
    
    iter = current.group->material_groups.find(name);
    if(iter == current.group->material_groups.end()) {
        current.group->material_groups[name] = LoaderMatGroup(name);
        current.mgroup = &(current.group->material_groups[name]);
    }else {
        current.mgroup = &(iter->second);
    }
    
    next.hasMtl = false;
}

/**************************************************/
