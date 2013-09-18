
#ifndef CS354_GENERIC_SHADER_HPP
#define CS354_GENERIC_SHADER_HPP

#include <stdio.h>
#include <string>
#include <vector>
#include <exception>
#include "../common.hpp"
#include "Material.hpp"

namespace cs354 {
    class Shader {
    public:
        /* Static interface */
        static void UseDefaultShaders();
        
        /* Non-static interface */
        Shader();
        ~Shader();
        
        void add(GLenum type, FILE *file);
        void add(GLenum type, std::string &progdata);
        void link();
        void use();
        
        /* Get opengl handle to shader. */
        GLuint handle();
        /* Get uniform location from this shader */
        GLint getUniform(const char *name);
        /* Get MaterialLocations. */
        const MaterialLocations & getLocations() const;
    private:
        MaterialLocations locations;
        GLuint program;
        std::vector<GLuint> shaders;
        bool linked;
    };
}

#endif
