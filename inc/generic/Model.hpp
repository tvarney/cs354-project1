
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
        MaterialGroup(const std::string &name);
        MaterialGroup(const std::string &name,
                      const Material &mat = Material::Default);
        ~MaterialGroup();
        
        /* The material name, used as the name of the group */
        const std::string name;
        /* The material reference. */
        const Material &mat;
        std::vector<GLuint> elements;
    };
    
    struct PolyGroup {
        PolyGroup(const std::string &name, const Model *owner);
        ~PolyGroup();
        
        MaterialGroup & getMatGroup(const char *name);
        MaterialGroup & getMatGroup(const std::string &name);
        
        std::string name;
        uint32_t flags;
        std::list<MaterialGroup> matgroups;
        const Model *owner;
    };
    
    class ModelParserState;
    class Model {
    public:
        Model();
        ~Model();
        
        PolyGroup & getGroup(const std::string &name);
        PolyGroup & getGroup(const char *name);
        
        void trim();
        void draw();
        
        const Material * getMaterial(const std::string &name) const;
        Translation getCenteredTranslation() const;
        GLfloat getScaleFactor(GLfloat width, GLfloat height,
                               GLfloat depth) const;
        
        friend class ModelParserState;
    protected:
        GLfloat min_x, max_x;
        GLfloat min_y, max_y;
        GLfloat min_z, max_z;
        
        std::vector<GLfloat> vertices; /*< Triplet */
        std::vector<GLfloat> normals; /*< Triplet */
        std::vector<GLfloat> texture; /*< Pair */
        std::list<PolyGroup> groups;
        std::map<std::string, Material> materials;
    };
}

#endif
