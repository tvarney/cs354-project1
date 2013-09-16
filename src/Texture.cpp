
#include "generic/Texture.hpp"

#include "common.hpp"
#include "generic/Image.hpp"

#include <exception>
#include <stdexcept>

using namespace cs354;

Texture::Texture(const Image &img) {
    glGenTextures(1,&handle);
    if(handle == 0) {
        throw std::runtime_error(std::string("Could not create texture"));
    }
    
    int format = img.glFormat();
    uint32_t width = img.getWidth(), height = img.getHeight();
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, img.getData());
    GLenum error = glGetError();
    if(error != GL_NO_ERROR) {
        glDeleteTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, 0);
        throw std::runtime_error(std::string("Could not upload texture data"));
    }
}
Texture::~Texture() {
    glDeleteTextures(1, &handle);
}
