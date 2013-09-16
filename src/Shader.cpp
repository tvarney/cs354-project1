
/**
 * Shader
 * Code for creating, compiling and linking OpenGL shader programs. This is
 * intended as a convience class; instead of needing to remember all of the
 * steps for shader creation, a user of the class just needs to remember to
 * add shaders and call link().
 * 
 * Author: Troy Varney - tav285 [troy.a.varney@gmail.com]
 */

#include "generic/Shader.hpp"
#include <sstream>
#include <stdio.h>
#include <stdexcept>

using namespace cs354;

Shader::Shader() :
    program(0), linked(false)
{ }
Shader::~Shader() {
    if(linked) {
        for(size_t i = 0; i < shaders.size(); ++i) {
            glDetachShader(program, shaders[i]);
            glDeleteShader(shaders[i]);
        }
    }else {
        for(size_t i = 0; i < shaders.size(); ++i) {
            glDeleteShader(shaders[i]);
        }
    }
}

void Shader::add(GLenum type, FILE *file) {
    if(linked) {
        throw std::runtime_error(std::string("Can't add to a linked shader"));
    }
    
    char buff[1025];
    size_t read;
    std::stringstream data(std::stringstream::in | std::stringstream::out);
    if(!file) {
        throw std::runtime_error(std::string("Invalid File Pointer"));
    }
    
    while((read = fread(buff, 1, 1024, file)) > 0) {
        buff[read] = '\0';
        data << buff;
    }
    std::string str = data.str();
    this->add(type, str);
}

void Shader::add(GLenum type, std::string &progdata) {
    if(linked) {
        throw std::runtime_error(std::string("Can't add to a linked shader"));
    }
    
    GLuint shader;
    GLint status, logLen;
    const char *pdata, *strShaderType;
    GLchar *infoLog;
    std::stringstream err(std::stringstream::in | std::stringstream::out);
    if(type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER) {
        throw std::runtime_error(std::string("Invalid Shader Type"));
    }
    shader = glCreateShader(type);
    pdata = progdata.data();
    glShaderSource(shader, 1, &pdata, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        infoLog = new GLchar[logLen + 1];
        glGetShaderInfoLog(shader, logLen, NULL, infoLog);
        strShaderType = NULL;
        switch(type) {
        case GL_VERTEX_SHADER:
            strShaderType = "Vertex Shader";
            break;
        case GL_FRAGMENT_SHADER:
            strShaderType = "Fragment Shader";
            break;
        }
        err << "Error compiling " << strShaderType << " :" << std::endl;
        err << infoLog << std::endl;
        delete[] infoLog;
        throw std::runtime_error(err.str());
    }
    
    shaders.push_back(shader);
}

void Shader::link() {
    if(linked) {
        return;
    }
    
    GLint status, logLen;
    GLchar *infoLog;
    size_t i, ssize;
    std::stringstream err(std::stringstream::in | std::stringstream::out);
    
    ssize = shaders.size();
    if(ssize < 1) {
        throw std::runtime_error(std::string("No shaders to link"));
    }
    
    program = glCreateProgram();
    for(i = 0; i < ssize; ++i) {
        glAttachShader(program, shaders[i]);
    }
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        infoLog = new GLchar[logLen + 1];
        glGetProgramInfoLog(program, logLen, NULL, infoLog);
        err << "Linker Error:" << std::endl;
        err << infoLog << std::endl;
        delete[] infoLog;
        throw std::runtime_error(err.str());
    }
    linked = true;
}

void Shader::use() {
    glUseProgram(program);
}

GLuint Shader::handle() {
    return program;
}

GLint Shader::getUniform(const char *name) {
    return glGetUniformLocation(program, name);
}

void Shader::UseDefaultShaders() {
    glUseProgram(0);
}
