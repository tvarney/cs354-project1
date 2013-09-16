
#include "generic/Model.hpp"

#include <cstdio>

using namespace cs354;

Material Material::Default;
GLint Material::loc_ka, Material::loc_kd, Material::loc_ks;
GLint Material::loc_tr, Material::loc_ns;

void Material::GetLocations(GLuint shader) {
    loc_ka = glGetUniformLocation(shader, "Ka");
    loc_kd = glGetUniformLocation(shader, "Kd");
    loc_ks = glGetUniformLocation(shader, "Ks");
    loc_tr = glGetUniformLocation(shader, "Tr");
    loc_ns = glGetUniformLocation(shader, "Ns");
    /* Bind default material to the new shader locations */
    Material::Default.bind();
}

Material::Material() {
    ka[0] = ka[1] = ka[2] = 1.0;
    kd[0] = kd[1] = kd[2] = 0.9;
    ks[0] = ks[1] = ks[2] = 0.8;
    tr = 1.0;
    ns = 0.0;
    illum = 1;
    map_ka = map_kd = map_ks = map_d = 0;
    decal = bump = 0;
}
Material::~Material() { }

Material & Material::operator=(const Material &rhs) {
    ka[0] = rhs.ka[0];
    ka[1] = rhs.ka[1];
    ka[2] = rhs.ka[2];
    
    kd[0] = rhs.kd[0];
    kd[1] = rhs.kd[1];
    kd[2] = rhs.kd[2];
    
    ks[0] = rhs.ks[0];
    ks[1] = rhs.ks[1];
    ks[2] = rhs.ks[2];
    
    tr = rhs.tr;
    ns = rhs.ns;
    illum = rhs.illum;
    map_ka = rhs.map_ka;
    map_kd = rhs.map_kd;
    map_ks = rhs.map_ks;
    map_d = rhs.map_d;
    decal = rhs.decal;
    bump = rhs.bump;
    return (*this);
}

void Material::bind() {
    if(Material::loc_ka != -1) {
        glUniform3f(Material::loc_ka, ka[0], ka[1], ka[2]);
    }
    if(Material::loc_kd != -1) {
        glUniform3f(Material::loc_kd, kd[0], kd[1], kd[2]);
    }
    if(Material::loc_ks != -1) {
        glUniform3f(Material::loc_ks, ks[0], ks[1], ks[2]);
    }
    if(Material::loc_tr != -1) {
        glUniform1f(Material::loc_tr, tr);
    }
    if(Material::loc_ns != -1) {
        glUniform1f(Material::loc_ns, ns);
    }
}

MaterialGroup::MaterialGroup(const std::string &name, const Material &mat) :
    name(name), mat(mat)
{ }
MaterialGroup::~MaterialGroup() { }

PolyGroup::PolyGroup(const std::string &name, const Model *owner) :
    name(name), owner(owner)
{ }
PolyGroup::~PolyGroup() { }

MaterialGroup & PolyGroup::getMatGroup(const char *name) {
    return this->getMatGroup(std::string(name));
}
MaterialGroup & PolyGroup::getMatGroup(const std::string &name) {
    std::list<MaterialGroup>::iterator iter = matgroups.begin();
    for(; iter != matgroups.end(); ++iter) {
        if((*iter).name.compare(name) == 0) {
            return *iter;
        }
    }
    
    const Material *mat = owner->getMaterial(name);
    if(mat == NULL) {
        fprintf(stderr, "Warning: Invalid material reference: %s\n",
                name.c_str());
        mat = &(Material::Default);
    }
    MaterialGroup newgroup(name, *mat);
    matgroups.push_back(newgroup);
    return matgroups.back();
}

Model::Model() { }
Model::~Model() { }

PolyGroup & Model::getGroup(const std::string &name) {
    std::list<PolyGroup>::iterator iter = groups.begin();
    for(; iter != groups.end(); ++iter) {
        if((*iter).name.compare(name) == 0) {
            return *iter;
        }
    }
    groups.push_back(PolyGroup(name, this));
    return groups.back();
}
PolyGroup & Model::getGroup(const char *name) {
    return this->getGroup(std::string(name));
}

/* Predicates for empty polygroups and materialgroups, used below */
static bool pg_is_empty(const PolyGroup &group) {
    return (group.matgroups.size() == 0);
}
static bool mg_is_empty(const MaterialGroup &mgroup) {
    return (mgroup.elements.size() == 0);
}

/* Remove any empty material groups, then remove any poly groups without
 * material groups. Not the fastest way to do this type of thing, but simple.
 */
void Model::trim() {
    std::list<MaterialGroup>::iterator miter;
    std::list<PolyGroup>::iterator iter;
    
    /* Remove empty material groups using above predicate for each polygroup */
    for(iter = groups.begin(); iter != groups.end(); ++iter) {
        (*iter).matgroups.remove_if(mg_is_empty);
    }
    /* Remove empty polygroups using above predicate */
    groups.remove_if(pg_is_empty);
}

void Model::draw() {
    size_t vsize = vertices.size();
    /* Enable arrays only for what we will use. */
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices.data());
    bool do_norm = (normals.size() == vsize);
    bool do_tex = (texture.size() == vsize);
    if(do_norm) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, normals.data());
    }
    if(do_tex) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3,GL_FLOAT,0,texture.data());
    }
    
    /* Iterate over every group and sub-group, drawing them */
    std::list<PolyGroup>::iterator iter, group_end = groups.end();
    std::list<MaterialGroup>::iterator miter, mat_end;
    for(iter = groups.begin(); iter != group_end; ++iter) {
        PolyGroup &group = (*iter);
        mat_end = group.matgroups.end();
        for(miter = group.matgroups.begin(); miter != mat_end; ++miter) {
            glDrawElements(GL_TRIANGLES, (*miter).elements.size(),
                           GL_UNSIGNED_INT, &((*miter).elements[0]));
        }
    }
    
    /* Disable any used arrays. */
    if(do_tex) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if(do_norm) {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    glDisable(GL_VERTEX_ARRAY);
}

const Material * Model::getMaterial(const std::string &name) const {
    /* Names like this are why the 'auto' keyword was introduced
     * Find the material in our material map.
     */
    std::map<std::string, Material>::const_iterator mat_loc;
    mat_loc = materials.find(name);
    if(mat_loc == materials.end()) {
        return NULL;
    }
    /* Dereference the iterator, get a refernce. Take address of referenced
     * object.
     */
    return &(mat_loc->second);
}
