
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
/* Symbols for the bison generated parsers */
WavefrontLoader * cs354::loader;
/**************************************************/
/* Defines to make my life easier */
#define RuntimeError(msg) std::runtime_error(std::string(msg))

static const char _inv_mat_ref[] =
    "Attempt to set %s without material reference.\n";
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
    loader = this;
    parse(fname);
    return cache_to_model();
}
Model * WavefrontLoader::load(const char *fname, GLfloat max_dim) {
    loader = this;
    parse(fname);
    scale(max_dim);
    return cache_to_model();
}
Model * WavefrontLoader::load(const char *fname, Vertex origin) {
    loader = this;
    parse(fname);
    translate(origin);
    return cache_to_model();
}
Model * WavefrontLoader::load(const char *fname, Vertex origin,
                              GLfloat max_dim)
{
    loader = this;
    parse(fname);
    translate(origin);
    scale(max_dim);
    return cache_to_model();
}

void WavefrontLoader::use(std::map<std::string, Material> & global_mat_map) {
    globalMaterialMap = &global_mat_map;
}

/**************************************************/
/* Parser interface */

void WavefrontLoader::v(GLfloat coords[3]) {
    vertices.push_back(Vertex(coords));
    /* Update stats for scale/transform if requested */
    max.x = (coords[0] > max.x ? coords[0] : max.x);
    min.x = (coords[0] < min.x ? coords[0] : min.x);
    max.y = (coords[1] > max.y ? coords[1] : max.y);
    min.y = (coords[1] < min.y ? coords[1] : min.y);
    max.z = (coords[2] > max.z ? coords[2] : max.z);
    min.z = (coords[2] < min.z ? coords[2] : min.z);
}
void WavefrontLoader::vn(GLfloat coords[3]) {
    normals.push_back(Normal(coords));
}
void WavefrontLoader::vt(GLfloat coords[3]) {
    texCoords.push_back(TextureCoord(coords));
}
extern int wf_lineno;
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
        log("Error on line %d: Too few arguments to f [%d]\n", wf_lineno,
            int(fs_size));
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
    faceStack.clear();
}
void WavefrontLoader::fArg(int args[3]) {
    faceStack.push_back(Element(args));
}
void WavefrontLoader::mtllib(const char *lib) {
    /* Here is the fun part, we call a new parser from within another parser */
    libname = basename + std::string("/") + std::string(lib);
    FILE *libfp = fopen(libname.c_str(), "r");
    if(!libfp) {
        log("Could not open mtllib %s\n", libname.c_str());
        return;
    }
    
    mat_restart(libfp);
    int rval;
    try {
        rval = mat_parse();
    }catch(std::exception &e) {
        log("Could not parse %s; %s\n", libname.c_str(), e.what());
        fclose(libfp);
        throw e;
    }
    
    fclose(libfp);
    
    if(mat.valid) {
        materials[mat.name] = mat.def;
    }
    
    switch(rval) {
    case 0:
        break;
    case 1:
        log("Syntax error while parsing %s\n", libname.c_str());
        throw RuntimeError("Syntax Error");
    case 2:
        log("Material parser exhausted memory while parsing %s\n",
            libname.c_str());
        throw RuntimeError("Parser exhausted memory");
    default:
        log("Unknown error parsing %s\n", libname.c_str());
        throw RuntimeError("Unknown Error");
    }
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
        materials[mat.name] = mat.def;
    }
    
    /* Set the new name, then make sure that the material hasn't already been
     * defined. */
    mat.name = mtlname;
    
    if(materials.find(mat.name) != materials.end()) {
        log("Redefinition of material \"%s\"\n", mtlname);
        mat.def = materials[mat.name];
    }else if(globalMaterialMap != NULL && globalMaterials &&
             globalMaterialMap->find(mat.name) != globalMaterialMap->end())
    {
        log("Redefinition of material \"%s\"in global material map.\n",
            mtlname);
        mat.def = globalMaterialMap->find(mat.name)->second;
    }else {
        mat.def = Material::Default;
    }
    mat.valid = true;
}

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
void WavefrontLoader::parse(const char *file_name) {
    int rval;
    clear();
    
    FILE *fp = fopen(file_name, "r");
    if(!fp) {
        /* Couldn't open the file, throw an exception to abort. */
        throw RuntimeError("Could not open file");
    }
    
    fname = file_name;
    size_t last_sep = fname.rfind("/");
    if(last_sep == std::string::npos) {
        basename = ".";
    }else {
        basename = std::string(fname, 0, last_sep);
    }
    
    /* Call the parser; this may throw an exception due to the wf_parse method
     * calling methods of WavefrontLoader that throw exceptions. To be safe,
     * any exceptions are caught, the file is closed, then the exception is
     * rethrown. */
    wf_restart(fp);
    try {
        rval = wf_parse();
    }catch(std::runtime_error &re) {
        fclose(fp);
        throw re;
    }catch(std::exception &e) {
        fclose(fp);
        throw e;
    }
    
    /* Close the file - done here so that if the parser returned an error code
     * we don't leave the file open. */
    fclose(fp);
    
    /* Examine the return value. 0 = good, 1 = Invalid Syntax,
     * 2 = Out of Memory */
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
}

void WavefrontLoader::scale(GLfloat maxdim) {
    size_t nvertices = vertices.size();
    if(nvertices <= 1) {
        return;
    }
    
    GLfloat diffx = max.x - min.x;
    GLfloat diffy = max.y - min.y;
    GLfloat diffz = max.z - min.z;
    GLfloat maxdiff = (diffx > diffy ? diffx : diffy);
    maxdiff = (maxdiff > diffz ? maxdiff : diffz);
    GLfloat scalefactor = maxdim / maxdiff;
    
    for(size_t i = 0; i < nvertices; ++i) {
        /* Translate point to vector from origin, scale vector, convert back to
         * point. */
        vertices[i] = (vertices[i].toVector() * scalefactor).toPoint();
    }
    
    max.x *= scalefactor;
    max.y *= scalefactor;
    max.z *= scalefactor;
    min.x *= scalefactor;
    min.y *= scalefactor;
    min.z *= scalefactor;
}
void WavefrontLoader::translate(Vertex origin) {
    size_t nvertices = vertices.size();
    if(nvertices <= 1) {
        return;
    }
    
    /* Compute the translation required to center around origin */
    Vector<GLfloat> translation(max.x - min.x, max.y - min.y, max.z - min.z);
    translation *= 0.5f;
    translation += Vector<GLfloat>(min.x, min.y, min.z);
    translation *= -1.0f;
    /* Get vector to origin point, add it to our calculated translation */
    /*TODO: Check my math. I think this results in the correct vector */
    translation += origin.toVector();
    
    for(size_t i = 0; i < nvertices; ++i) {
        vertices[i] += translation;
    }
    max.x += translation.vx;
    max.y += translation.vy;
    max.z += translation.vz;
    min.x += translation.vx;
    min.y += translation.vy;
    min.z += translation.vz;
}

typedef std::map<std::string, LoaderObject>::const_iterator lo_iter;
typedef std::map<std::string, LoaderGroup>::const_iterator lg_iter;
typedef std::map<std::string, LoaderMatGroup>::const_iterator lmg_iter;
Model * WavefrontLoader::cache_to_model() {
    size_t nobjects = 0, ngroups = 0, nelements = 0;
    Model * model = new Model();
    
    /* Copy model materials over to the newly created model */
    model->materials = materials;
    
    GLuint current_element = 0, elementid;
    /* Our map to map distinct elements, allows us to not put redundant
     * elements in our model arrays. */
    std::map<Element, GLuint> elements;
    
    /* Copy elements, ensuring that each element triple corresponds to a single
     * index in the model. This is...annoying to do. */
    lo_iter lobj_iter, lobj_end;
    lg_iter lgroup_iter, lgroup_end;
    lmg_iter lmgroup_iter, lmgroup_end;
    std::map<Element, GLuint>::iterator element_iter;
    
    for(lobj_iter = objects.begin(); lobj_iter != objects.end(); ++lobj_iter) {
        nobjects += 1;
        /* Get current LoaderObject and create a model object to correspond */
        const LoaderObject &lobj = lobj_iter->second;
        Object &object = model->get(lobj_iter->first);
        
        lgroup_end = lobj.groups.end();
        lgroup_iter = lobj.groups.begin();
        for(; lgroup_iter != lgroup_end; ++lgroup_iter) {
            ngroups += 1;
            /* Get current LoaderGroup and create model group to correspond */
            const LoaderGroup &lgroup = lgroup_iter->second;
            Group &group = object.get(lgroup_iter->first);
            
            lmgroup_end = lgroup.material_groups.end();
            lmgroup_iter = lgroup.material_groups.begin();
            for(; lmgroup_iter != lmgroup_end; ++lmgroup_iter) {
                /* Get current Material Group, create Model Material Group */
                const LoaderMatGroup &lmgroup = lmgroup_iter->second;
                MaterialGroup &mgroup = group.get(lmgroup.mtlname);
                
                size_t ntri = lmgroup.faces.size();
                nelements += ntri;
                for(size_t i = 0; i < ntri; ++i) {
                    Triangle tri = lmgroup.faces[i];
                    /* Do the delayed invalidation requested by resolve() */
                    if(invalidate_texcoords) {
                        tri.v1.vt = -1;
                        tri.v2.vt = -1;
                        tri.v3.vt = -1;
                    }
                    if(invalidate_normals) {
                        tri.v1.vn = -1;
                        tri.v2.vn = -1;
                        tri.v3.vn = -1;
                    }
                    
                    element_iter = elements.find(tri.v1);
                    if(element_iter == elements.end()) {
                        elementid = current_element;
                        current_element++;
                        elements[tri.v1] = elementid;
                        
                        push_element(tri.v1, model);
                    }else {
                        elementid = element_iter->second;
                    }
                    mgroup.elements.push_back(elementid);
                    
                    element_iter = elements.find(tri.v2);
                    if(element_iter == elements.end()) {
                        elementid = current_element;
                        current_element++;
                        elements[tri.v2] = elementid;
                        
                        push_element(tri.v2, model);
                    }else {
                        elementid = element_iter->second;
                    }
                    mgroup.elements.push_back(elementid);
                    
                    element_iter = elements.find(tri.v3);
                    if(element_iter == elements.end()) {
                        elementid = current_element;
                        current_element++;
                        elements[tri.v3] = elementid;
                        
                        push_element(tri.v3, model);
                    }else {
                        elementid = element_iter->second;
                    }
                    mgroup.elements.push_back(elementid);
                }
            }
        }
    }
    
    /* Display model statistics */
    log("Model Statistics:\n");
    log("    # Objects: %llu\n", (unsigned long long)nobjects);
    log("     # Groups: %llu\n", (unsigned long long)ngroups);
    log("  # Materials: %llu\n",(unsigned long long)(model->materials.size()));
    log("   # Vertices: %llu\n",
        (unsigned long long)(model->vertices.size()) / 3);
    log(" # Tex Coords: %llu\n",
        (unsigned long long)(model->texture.size()) / 2);
    log("    # Normals: %llu\n",
        (unsigned long long)(model->normals.size()) / 3);
    log("  # Triangles: %llu\n", (unsigned long long)nelements);
    return model;
}

/*NOTE: could be renamed to 'reset', as that encompasses it's function better
 */
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
    
    invalidate_texcoords = invalidate_normals = false;
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
        invalidate_texcoords = true;
    }else {
        e.vt -= 1;
    }
    
    if(e.vn < 0) {
        e.vn = normals.size() + e.vn;
    }else if(e.vn == 0) {
        e.vn = -1;
        invalidate_normals = true;
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
            if(globalMaterials && globalMaterialMap != NULL) {
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

void WavefrontLoader::push_element(const Element &e, Model *mptr) {
    if((unsigned long long)e.v >= vertices.size()) {
        clear();
        throw RuntimeError("Invalid Vertex index\n");
    }
    
    mptr->vertices.push_back(vertices[e.v].x);
    mptr->vertices.push_back(vertices[e.v].y);
    mptr->vertices.push_back(vertices[e.v].z);
    if(!invalidate_texcoords) {
        if((unsigned long long)e.vt >= texCoords.size()) {
            log("Invalidating Textures: texture index out of bounds\n");
            invalidate_texcoords = true;
            mptr->texture.clear();
        }else {
            mptr->texture.push_back(texCoords[e.vt].x);
            mptr->texture.push_back(texCoords[e.vt].y);
        }
    }
    if(!invalidate_normals) {
        if((unsigned long long)e.vn >= normals.size()) {
            log("Invalidating Normals: normal index out of bounds\n");
            invalidate_normals = true;
            mptr->normals.clear();
        }else {
            mptr->normals.push_back(normals[e.vn].vx);
            mptr->normals.push_back(normals[e.vn].vy);
            mptr->normals.push_back(normals[e.vn].vz);
        }
    }
}

/**************************************************/
