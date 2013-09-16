
#ifndef CS354_GENERIC_TEXTURE_HPP
#define CS354_GENERIC_TEXTURE_HPP

#include <stdint.h>

namespace cs354 {
    class Image;
    class Texture {
    public:
        Texture(const Image &img);
        ~Texture();
        
        uint32_t getHandle() const;
    protected:
        uint32_t handle;
    };
}

#endif
