
#ifndef CS354_GENERIC_IMAGE_IO_HPP
#define CS354_GENERIC_IMAGE_IO_HPP

#include "Image.hpp"
#include <cstdio>

namespace cs354 {
    /* ImageIO is stateless, so no need to have a class; a namespace will do */
    namespace ImageIO {
        Image * Load(const char *fname);
        Image * Load(FILE *fp);
        Image * LoadBMP(FILE *fp);
        Image * LoadPNG(FILE *fp);
        Image * LoadTGA(FILE *fp);
    };
}

#endif
