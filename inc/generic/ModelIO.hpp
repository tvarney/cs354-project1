
#ifndef CS354_GENERIC_MODEL_IO_HPP
#define CS354_GENERIC_MODEL_IO_HPP

#include <list>
#include <map>
#include <stdio.h>
#include <vector>
#include "Model.hpp"
#include "../common.hpp"

namespace cs354 {
    struct FaceArg {
        FaceArg(int v, int vn, int vt);
        int v, vn, vt;
    };
    extern class ModelParserState {
    public:
        ModelParserState(Model *ptr = NULL);
        ~ModelParserState();
        
        /* Attach a model to this parser state. The parsing functions will
         * then fill this model. This will clear any data left in the parser
         * state.
         */
        void attach(Model &model);
        
        /* Add a vertex to the current model */
        void vertex(GLfloat coords[3]);
        /* Add a normal to the current model */
        void normal(GLfloat coords[3]);
        /* Add a texture coordinate to the current model */
        void texture(GLfloat coords[3]);
        /* Empty the face element stack to create a new set of polygons to the
         * model.
         */
        void face();
        /* Add an element to the face element stack */
        void face_arg(int indices[3]);
        /* Load a material library into the current parser state. */
        void mtllib(const char *libname);
        /* Look up and use the given material for faces. */
        void usemtl(const char *mtlname);
        /* Create a new group */
        void group(const char *groupname);
    private:
        /* Resolves the given ID. This may cause issues if both tesselation and
         * negative indices are used in the same file */
        int resolve(int id);
        
        bool negative_id, tesselations;
        std::vector<FaceArg> face_args;
        std::string next_mtl;
        Model *ptr;
        PolyGroup *current_group;
        MaterialGroup *current_mgroup;
        
        std::map<std::string, Material> materials;
    } model_parser_state;
    
    namespace ModelIO {
        Model * Load(const char *fname);
        Model * Load(FILE *fp, const char *fname = NULL);
    };
}

#endif
