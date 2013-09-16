
/**
 * Image:
 * Class that represents an image in RAM. These are used as a halfway point
 * between images on disc and textures, as well as a generic place for
 * calculated textures to be created.
 * 
 * Author: Troy Varney - tav285 [troy.a.varney@gmail.com]
 */

#include "generic/Image.hpp"

#include "common.hpp"

using namespace cs354;

int Image::BytesPerPixel(PixelFormat format) {
    switch(format) {
    case PF_RGB:
    case PF_BGR:
        return 3;
    case PF_RGBA:
    case PF_BGRA:
        return 4;
    case PF_GREY:
        return 1;
    }
    return -1;
}

Image::Image(uint32_t width, uint32_t height, PixelFormat format) :
    width(width), height(height), pitch(width), format(format)
{
    int bpp = Image::BytesPerPixel(format);
    data = new uint8_t[height * pitch * bpp];
}
Image::Image(uint32_t width, uint32_t height, uint32_t pitch,
             PixelFormat format) :
    width(width), height(height), pitch(pitch), format(format)
{
    int bpp = Image::BytesPerPixel(format);
    data = new uint8_t[height * pitch * bpp + 15];
}
Image::~Image() {
    delete data;
}

uint32_t Image::getWidth() const {
    return width;
}
uint32_t Image::getHeight() const {
    return height;
}
uint32_t Image::getPitch() const {
    return pitch;
}
PixelFormat Image::getFormat() const {
    return format;
}
int Image::glFormat() const {
    switch(format) {
    case PF_RGB:
        return GL_RGB;
        break;
    case PF_RGBA:
        return GL_RGBA;
        break;
    case PF_BGR:
        return GL_BGR;
        break;
    case PF_BGRA:
        return GL_BGRA;
        break;
    case PF_GREY:
        return GL_RED;
        break;
    }
    return -1;
}

const uint8_t * const Image::getData() const {
    return data;
}
uint8_t * Image::getMutableData() {
    return data;
}
