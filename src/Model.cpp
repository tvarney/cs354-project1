
#include "generic/Model.hpp"

#include <cstdio>
#include <cfloat>

using namespace cs354;

MaterialGroup::MaterialGroup(const std::string &name, const Material &mat) :
    name(name), mat(mat)
{ }
MaterialGroup::~MaterialGroup() { }

Group::Group(const std::string &name, const Model *owner) :
    name(name), owner(owner)
{ }
Group::~Group() { }

MaterialGroup & Group::get(const char *name) {
    return this->get(std::string(name));
}
MaterialGroup & Group::get(const std::string &name) {
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
    matgroups.push_back(MaterialGroup(name, *mat));
    return matgroups.back();
}

Object::Object(const std::string &name, const Model *owner) :
    name(name), owner(owner)
{ }
Object::~Object() { }

Group & Object::get(const char *name) {
    return this->get(std::string(name));
}
Group & Object::get(const std::string &name) {
    std::list<Group>::iterator iter = groups.begin();
    for(; iter != groups.end(); ++iter) {
        if((*iter).name.compare(name) == 0) {
            return *iter;
        }
    }
    groups.push_back(Group(name, owner));
    return groups.back();
}


Model::Model() { }
Model::~Model() { }

Object & Model::get(const std::string &name) {
    std::list<Object>::iterator iter = objects.begin();
    for(; iter != objects.end(); ++iter) {
        if((*iter).name.compare(name) == 0) {
            return *iter;
        }
    }
    objects.push_back(Object(name, this));
    return objects.back();
}
Object & Model::get(const char *name) {
    return this->get(std::string(name));
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
    std::list<Object>::iterator obj_iter, obj_end = objects.end();
    std::list<Group>::iterator group_iter, group_end;
    std::list<MaterialGroup>::iterator mat_iter, mat_end;
    for(obj_iter = objects.begin(); obj_iter != obj_end; ++obj_iter) {
        Object &object = *obj_iter;
        group_end = object.groups.end();
        group_iter = object.groups.begin();
        for(; group_iter != group_end; ++group_iter) {
            Group &group = *group_iter;
            mat_end = group.matgroups.end();
            mat_iter = group.matgroups.begin();
            for(; mat_iter != mat_end; ++mat_iter) {
                MaterialGroup &mgroup = *mat_iter;
                mgroup.mat.bind();
                glDrawElements(GL_TRIANGLES, mgroup.elements.size(),
                               GL_UNSIGNED_INT, &(mgroup.elements[0]));
            }
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
