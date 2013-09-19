
#ifndef CS354_GENERIC_WAVEFRONT_LOADER_HPP
#define CS354_GENERIC_WAVEFRONT_LOADER_HPP

/* This loader will have trouble with very large models, but I don't plan on
 * loading several hundred megabyte models anyways.
 */

#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "Material.hpp"

namespace cs354 {
    class Model;
    
    struct Element {
        Element(int args[3]);
        ~Element();
        
        int v, vt, vn;
    };
    struct Vertex {
        Vertex(GLfloat args[3]);
        ~Vertex();
        
        GLfloat x, y, z;
    };
    struct TextureCoord {
        TextureCoord(GLfloat args[3]);
        ~TextureCoord();
        GLfloat u, v, w;
    };
    struct Normal {
        Normal(GLfloat args[3]);
        ~Normal();
        
        GLfloat xn, yn, zn;
    };
    struct Triangle {
        Element v1, v2, v3;
    };

    struct LoaderMatGroup {
        LoaderMatGroup();
        LoaderMatGroup(const std::string &mtlname);
        ~LoaderMatGroup();
        
        std::string mtlname;
        std::vector<Triangle> faces;
    };
    struct LoaderGroup {
        LoaderGroup();
        LoaderGroup(const std::string &name);
        ~LoaderGroup();
        
        std::string name;
        std::map<std::string, LoaderMatGroup> material_groups;
    };
    struct LoaderObject {
        LoaderObject();
        LoaderObject(const std::string &name);
        ~LoaderObject();
        
        std::string name;
        std::map<std::string, LoaderGroup> groups;
    };
    
    class WavefrontLoader {
    public:
        WavefrontLoader(bool keep_materials = false,
                        bool global_mats = false);
        ~WavefrontLoader();
        
        /* Loads a model from the given file. If fp == NULL, then the function
         * will attempt to open the file given by fname.
         */
        Model * load(const char *fname, FILE *fp = NULL);
        
        /* Set global Material table */
        void use(std::map<std::string, Material> & global_mat_map);
        
        /* Interface for adding things from the parser. */
        void v(GLfloat coords[3]);
        void vn(GLfloat coords[3]);
        void vt(GLfloat coords[3]);
        void f();
        void fArg(int indices[3]);
        void mtllib(const char *libname);
        void usemtl(const char *mtlname);
        void g(const char *groupname);
        void o(const char *objectname);
        
        void newmtl(const char *mtlname);
        void ka(GLfloat color[3]);
        void kd(GLfloat color[3]);
        void ks(GLfloat color[3]);
        void ns(GLfloat amount);
        void tr(GLfloat amount);
    private:
        /* Helper function to clear out data */
        void clear();
        void log(const char *msg, ...);
        void resolve(Element &e);
        void newObject(const std::string &name);
        void newGroup(const std::string &name);
        void newMatGroup(const std::string &name);
        
        /* The file being loaded. The loader is not thread safe; a seperate
         * instance is required for each thread.
         */
        std::string fname;
        FILE *fp;
        
        /* Logging file pointer */
        FILE *logFile;
        
        /* Loader cache */
        std::vector<Element> faceStack;
        std::vector<Vertex> vertices;
        std::vector<TextureCoord> texCoords;
        std::vector<Normal> normals;
        
        /* The model as loaded from the .obj file. This isn't guaranteed to
         * be valid */
        std::map<std::string, LoaderObject> objects;
        
        /* Material Maps. If the material named cannot be found in the local
         * map, look in the global map.
         */
        std::map<std::string, Material> materials;
        std::map<std::string, Material> * globalMaterialMap;
        
        /* Look-ahead values for materials that are defined before starting a
         * group.
         * If hasNextMtl is true, when f() is called it will update the current
         * LoaderMatGroup to use the given value.
         */
        struct {
            bool hasMtl, hasGroup, hasObject;
            std::string mtl, group, object;
        } next;
        
        /* Local Material buffer, used when reading material files */
        Material matdef;
        
        /* Material behavior flags.
         * keepMaterials will cause the Loader to save materials between runs.
         * globalMaterials will cause the Loader to put materials into the
         * global namespace if it exists.
         */
        bool keepMaterials, globalMaterials;
        
        struct {
            LoaderMatGroup *mgroup;
            LoaderGroup *group;
            LoaderObject *object;
        } current;
    };
    extern WavefrontLoader ModelLoader;
}

#endif
