
#ifndef CS354_GENERIC_WAVEFRONT_LOADER_HPP
#define CS354_GENERIC_WAVEFRONT_LOADER_HPP

/* This loader will have trouble with very large models, but I don't plan on
 * loading several hundred megabyte models anyways.
 */

#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "Geometry.hpp"
#include "Material.hpp"

namespace cs354 {
    class Model;
    
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
        
        /* Loads a model from the given file. */
        Model * load(const char *fname);
        Model * load(const char *fname, Vertex origin);
        Model * load(const char *fname, GLfloat max_dim);
        Model * load(const char *fname, Vertex origin, GLfloat max_dim);
        
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
        
        /* Unsupported Features */
        void vp(GLfloat coord[3]);
        void map_ka(const char *kamap);
        void map_kd(const char *kdmap);
        void map_ks(const char *ksmap);
        void map_tr(const char *trmap);
        void bump(const char *bumpmap);
        void decal(const char *decal);
    private:
        /* Helper function to clear out data */
        void parse(const char *fname);
        Model * cache_to_model();
        void scale(GLfloat maxdim);
        void translate(Vertex origin);
        void clear();
        void log(const char *msg, ...);
        void resolve(Element &e);
        void newObject(const std::string &name);
        void newGroup(const std::string &name);
        void newMatGroup(const std::string &name);
        
        /* File information */
        std::string fname, libname, basename;
        FILE *fp;
        
        /* Logging file pointer */
        FILE *logFile;
        
        /* Loader cache */
        std::vector<Element> faceStack;
        std::vector<Vertex> vertices;
        std::vector<TextureCoord> texCoords;
        std::vector<Normal> normals;
        
        /* Useful stats for calculating bounding box and centering transforms
         */
        struct {
            GLfloat x, y, z;
        } max, min;
        
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
        struct {
            bool valid;
            std::string name;
            Material def;
        } mat;
        
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
}

#endif
