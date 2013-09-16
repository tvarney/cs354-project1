
#ifndef CS354_GENERIC_TEXTURE_CACHE_HPP
#define CS354_GENERIC_TEXTURE_CACHE_HPP

#include <map>
#include <string>

namespace cs354 {
    class Texture;
    
    class TextureCache {
    public:
        TextureCache();
        ~TextureCache();
        
        Texture * get(const char *texname);
        Texture * get(const std::string &texname);
        
        /* This will unload and DELETE any textures stored in the cache.
         * Using any texture pointers obtained from the cache before clearing
         * it after a clear WILL segfault. DON'T BE THAT PERSON.
         */
        void clear();
    private:
        std::map<std::string, Texture*> texmap;
    };
    extern TextureCache tex_cache;
}

#endif
