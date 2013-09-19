
#ifndef CS354_GENERIC_MODEL_HPP
#define CS354_GENERIC_MODEL_HPP

#include "../common.hpp"
#include "Material.hpp"

#include <list>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

namespace cs354 {
    /* The wavefront object format sucks -. -
     * A model consists of groups of polygons which may have any number of
     * different materials associated with them.
     * The vertices, normals and texture coordinates are all a part of the base
     * model, but the elements composing the polygons are stored in the lowest
     * layer (the Material Group of the Polygon Group of the Model).
     * In any case, this file is a huge beast, as I define everything a model
     * needs in it. Technically, Materials can be used outside of a model
     * but I don't do so in this project so I left it here.
     */
    class Model;
    
    struct Translation {
        GLfloat x, y, z;
    };
    
    struct MaterialGroup {
        MaterialGroup(const std::string &name,
                      const Material &mat = Material::Default);
        ~MaterialGroup();
        
        /* The material name, used as the name of the group */
        const std::string name;
        /* The material reference. */
        const Material &mat;
        std::vector<GLuint> elements;
    };
    
    struct Group {
        Group(const std::string &name, const Model *owner);
        ~Group();
        
        MaterialGroup & get(const char *name);
        MaterialGroup & get(const std::string &name);
        
        std::string name;
        std::list<MaterialGroup> matgroups;
        const Model *owner;
    };
    struct Object {
        Object(const std::string &name, const Model *owner);
        ~Object();
        
        Group & get(const std::string &name);
        Group & get(const char *name);
        
        std::string name;
        std::list<Group> groups;
        const Model *owner;
    };
    
    class ModelParserState;
    class WavefrontLoader;
    class Model {
    public:
        Model();
        ~Model();
        
        Object & get(const std::string &name);
        Object & get(const char *name);
        
        void draw();
        
        const Material * getMaterial(const char *name) const;
        const Material * getMaterial(const std::string &name) const;
        
        friend class ModelParserState;
        friend class WavefrontLoader;
    protected:
        std::vector<GLfloat> vertices; /*< Triplet */
        std::vector<GLfloat> normals; /*< Triplet */
        std::vector<GLfloat> texture; /*< Pair */
        std::list<Object> objects;
        std::map<std::string, Material> materials;
    };
}

#endif
