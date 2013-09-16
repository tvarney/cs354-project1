
#ifndef CS354_GENERIC_MODEL_HPP
#define CS354_GENERIC_MODEL_HPP

#include "../common.hpp"
#include <list>
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
     */
    
    struct Material {
        static GLint loc_ka, loc_kd, loc_ks, loc_tr, loc_ns;
        static void GetLocations();
        
        Material();
        ~Material();
        
        Material & operator=(const Material &rhs);
        
        void bind();
        
        GLfloat ka[3], kd[3], ks[3];
        GLfloat tr, ns;
        int illum;
        GLuint map_ka, map_kd, map_ks, map_d;
        GLuint decal, bump;
    };

    struct MaterialGroup {
        MaterialGroup(std::string &name, Material mat);
        ~MaterialGroup();
        
        std::string name;
        Material mat;
        std::vector<GLuint> elements;
    };
    
    struct PolyGroup {
        PolyGroup(const std::string &name);
        ~PolyGroup();
        
        MaterialGroup & getMatGroup(const char *name);
        MaterialGroup & getMatGroup(const std::string &name);
        
        std::string name;
        uint32_t flags;
        std::list<MaterialGroup> matgroups;
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
        
        friend class ModelParserState;
    protected:
        std::vector<GLfloat> vertices; /*< Triplet */
        std::vector<GLfloat> normals; /*< Triplet */
        std::vector<GLfloat> texture; /*< Pair */
        std::list<PolyGroup> groups;
        
    };
}

#endif
