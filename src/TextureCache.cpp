
/**
 * Texture Cache:
 * Class methods for caching textures. These methods will return the texture
 * already in memory if it exists. Otherwise, they will attempt to load them
 * from the provided file name.
 * 
 * Author: Troy Varney - tav285 [troy.a.varney@gmail.com]
 */

#include "generic/TextureCache.hpp"

#include "generic/Image.hpp"
#include "generic/ImageIO.hpp"
#include "generic/Texture.hpp"

using namespace cs354;

TextureCache tex_cache;

TextureCache::TextureCache() { }
TextureCache::~TextureCache() { }

Texture * TextureCache::get(const char *texname) {
    return get(std::string(texname));
}
Texture * TextureCache::get(const std::string &texname) {
    std::map<std::string, Texture *>::iterator it;
    it = texmap.find(texname);
    if(it != texmap.end()) {
        return it->second;
    }
    
    /* Load the texture, add it to the map and return the new texture */
    Image *img = ImageIO::Load(texname.data());
    Texture *tex = new Texture(*img);
    texmap[texname] = tex;
    return tex;
}

void TextureCache::clear() {
    std::map<std::string, Texture *>::iterator it;
    for(it = texmap.begin(); it != texmap.end(); ++it) {
        delete it->second;
    }
    texmap.clear();
}
