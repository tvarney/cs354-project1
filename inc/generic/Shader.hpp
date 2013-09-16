
#ifndef CS354_GENERIC_SHADER_HPP
#define CS354_GENERIC_SHADER_HPP

#include <stdio.h>
#include <string>
#include <vector>
#include <exception>
#include "../common.hpp"

namespace cs354 {
    class Shader {
    public:
        Shader();
        ~Shader();
        
        void add(GLenum type, FILE *file);
        void add(GLenum type, std::string &progdata);
        void link();
        void use();
        
        GLuint handle();
        GLint getUniform(const char *name);
        
        static void UseDefaultShaders();
    private:
        GLuint program;
        std::vector<GLuint> shaders;
        bool linked;
    };
}

#endif
