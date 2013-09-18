
#ifndef CS354_GENERIC_MATERIAL_HPP
#define CS354_GENERIC_MATERIAL_HPP

#include "../common.hpp"

namespace cs354 {
    struct Material {
        static Material Default;
        
        Material & operator=(const Material &rhs);
        
        void bind() const;
        
        GLfloat ka[3], kd[3], ks[3];
        GLfloat tr, ns;
        int illum;
        GLuint map_ka, map_kd, map_ks, map_d;
        GLuint decal, bump;
    };
    
    class Shader;
    class MaterialLocations {
    public:
        /* Static interface */
        static bool Bound();
        static GLint Ka();
        static GLint Kd();
        static GLint Ks();
        static GLint Tr();
        static GLint Ns();
        
        /* Non-static interface */
        MaterialLocations();
        ~MaterialLocations();
        
        GLint loc_ka, loc_kd, loc_ks, loc_tr, loc_ns;
        friend class Shader;
    private:
        static void Bind(const Shader &shader);
        static void Unbind();
        static const MaterialLocations * current_locations;
    };
}

#endif
