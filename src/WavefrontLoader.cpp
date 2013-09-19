
#include "generic/WavefrontLoader.hpp"
#include "generic/Model.hpp"

#include <cfloat>
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
/**************************************************/

/**************************************************/
Element::Element(int args[3]) :
    v(args[0]), vt(args[1]), vn(args[2])
{ }
Element::~Element() { }
/**************************************************/

/**************************************************/
Vertex::Vertex(GLfloat x, GLfloat y, GLfloat z) :
    x(x), y(y), z(z)
{ }
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

Model * WavefrontLoader::load(const char *fname) {
    parse(fname);
    return cache_to_model();
}
Model * WavefrontLoader::load(const char *fname, GLfloat max_dim) {
    parse(fname);
    scale(max_dim);
    return cache_to_model();
}
Model * WavefrontLoader::load(const char *fname, Vertex origin) {
    parse(fname);
    translate(origin);
    return cache_to_model();
}
Model * WavefrontLoader::load(const char *fname, Vertex origin,
                              GLfloat max_dim)
{
    parse(fname);
    scale(max_dim);
    translate(origin);
    return cache_to_model();
}

void WavefrontLoader::use(std::map<std::string, Material> & global_mat_map) {
    globalMaterialMap = &global_mat_map;
}

/**************************************************/
/* Parser interface */

void WavefrontLoader::v(GLfloat coords[3]) {
    vertices.push_back(Vertex(coords));
    /* We need to update the max and min stats of the model */
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
        newObject(next.object);
    }else if(current.group == NULL || next.hasGroup) {
        newGroup(next.group);
    }else if(current.mgroup == NULL || next.hasMtl) {
        newMatGroup(next.mtl);
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
    if(next.hasMtl) {
        log("Multiple Material definition: Overwriting \"%s\" with \"%s\"\n",
            next.mtl.c_str(), mtlname);
    }
    
    next.mtl = mtlname;
    next.hasMtl = true;
}
void WavefrontLoader::g(const char *groupname) {
    if(next.hasGroup) {
        log("Multiple Group definition: Overwriting \"%s\" with \"%s\"\n",
            next.group.c_str(), groupname);
    }
    
    next.group = groupname;
    next.hasGroup = true;
}
void WavefrontLoader::o(const char *objectname) {
    if(next.hasObject) {
        log("Multiple Object definition: Overwriting \"%\" with \"%s\"\n",
            next.object.c_str(), objectname);
    }
    
    next.object = objectname;
    next.hasObject = true;
}

/* Material commands */
void WavefrontLoader::newmtl(const char *mtlname) {
    if(mat.valid) {
        
    }
    
    mat.name = mtlname;
    mat.def = Material::Default;
    mat.valid = true;
}
static const char _inv_mat_ref[] =
    "Attempt to set %s without material reference.\n";
void WavefrontLoader::ka(GLfloat color[3]) {
    if(mat.valid) {
        mat.def.ka[0] = color[0];
        mat.def.ka[1] = color[1];
        mat.def.ka[2] = color[2];
    }else {
        log(_inv_mat_ref, "Ka");
    }
}
void WavefrontLoader::kd(GLfloat color[3]) {
    if(mat.valid) {
        mat.def.kd[0] = color[0];
        mat.def.kd[1] = color[1];
        mat.def.kd[2] = color[2];
    }else {
        log(_inv_mat_ref, "Kd");
    }
}
void WavefrontLoader::ks(GLfloat color[3]) {
    if(mat.valid) {
        mat.def.ks[0] = color[0];
        mat.def.ks[1] = color[1];
        mat.def.ks[2] = color[2];
    }else {
        log(_inv_mat_ref, "Ks");
    }
}
void WavefrontLoader::ns(GLfloat amount) {
    if(mat.valid) {
        mat.def.ns = amount;
    }else {
        log(_inv_mat_ref, "Ns");
    }
}
void WavefrontLoader::tr(GLfloat amount) {
    if(mat.valid) {
        mat.def.tr = amount;
    }else {
        log(_inv_mat_ref, "Tr");
    }
}

/**************************************************/
/* Private methods of WavefrontLoader */
void WavefrontLoader::parse(const char *fname) {
    clear();
    
    
    /* Open the file if we need to. 'openfile' is defined to allow us to close
     * the file when we are done if we need to (as in, if we opened the file
     * in this method).
     */
    FILE *fp = fopen(fname, "r");
    if(!fp) {
        /* Couldn't open the file, throw an exception to abort. */
        throw RuntimeError("Could not open file");
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
    
    /* Close the file since we are done with it. */
    fclose(fp);
    fp = NULL;
}

void WavefrontLoader::scale(GLfloat maxdim) {

}
void WavefrontLoader::translate(Vertex origin) {

}

Model * WavefrontLoader::cache_to_model() {
    Model * model = NULL;
    
    return model;
}

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
    
    mat.valid = false;
    mat.name = "";
    max.x = max.y = max.z = -FLT_MAX;
    min.x = min.y = min.z = FLT_MAX;
    
    next.hasMtl = next.hasGroup = next.hasObject = false;
    next.mtl = next.group = next.object = "";
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
    
    newGroup(next.group);
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
    
    newMatGroup(next.mtl);
    next.hasGroup = false;
}

void WavefrontLoader::newMatGroup(const std::string &name) {
    /* Check if the material requested has been loaded
     * Invalid references will still create material groups, they will just
     * use the default material.
     * "" is used to represent the default material, which skips the checks
     * for the material in the material maps.
     */
    std::map<std::string, Material>::iterator miter;
    if(std::strcmp(name.c_str(), "") != 0) {
        miter = materials.find(name);
        if(miter == materials.end()) {
            if(globalMaterialMap != NULL) {
                miter = (*globalMaterialMap).find(name);
                if(miter == (*globalMaterialMap).end()) {
                    log("Invalid material reference: %s\n", name.c_str());
                }
            }else {
                log("Invalid material reference: %s\n", name.c_str());
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
    
    next.hasMtl = false;
}

/**************************************************/
