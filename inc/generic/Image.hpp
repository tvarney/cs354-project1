
#ifndef CS354_GENERIC_IMAGE_HPP
#define CS354_GENERIC_IMAGE_HPP

#include <stdint.h>

namespace cs354 {
    /* I don't feel like supporting more pixel formats right now. */
    enum PixelFormat {
        PF_RGB,
        PF_RGBA,
        PF_BGR,
        PF_BGRA,
        PF_GREY,
        PF_GRAY = PF_GREY
    };
    
    class Image {
    public:
        static int BytesPerPixel(PixelFormat format);
        
        Image(uint32_t width, uint32_t height, PixelFormat format);
        Image(uint32_t width, uint32_t height, uint32_t pitch,
              PixelFormat format);
        ~Image();
        
        uint32_t getWidth() const;
        uint32_t getHeight() const;
        uint32_t getPitch() const;
        PixelFormat getFormat() const;
        int glFormat() const;
        
        const uint8_t * const getData() const;
        uint8_t * getMutableData();
    protected:
        uint32_t width, height, pitch;
        PixelFormat format;
        uint8_t *data;
    };
}

#endif
