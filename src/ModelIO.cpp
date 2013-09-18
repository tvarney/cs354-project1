
/**
 * ModelIO:
 * Free functions for loading models from disc.
 * Author: Troy Varney - tav285 [troy.a.varney@gmail.com]
 */

#include "generic/ModelIO.hpp"
#include "generic/Model.hpp"

#include <cstring>
#include <cstdio>
#include <climits>
#include <cstdlib>

using namespace cs354;

#define RuntimeException(msg) std::runtime_error(std::string(msg))
/* Used to simplify copying triples */
#define COPY_TRIPLE(from, to) \
    (to)[0] = (from)[0];      \
    (to)[1] = (from)[1];      \
    (to)[2] = (from)[2]
#define PUSH_TRIPLE(container, triple)          \
    (container).push_back(triple[0]);           \
    (container).push_back(triple[1]);           \
    (container).push_back(triple[2])

ModelParserState cs354::model_parser_state;
static bool has_warned_mismatch = false;
extern int wf_parse(void);
extern void wf_restart(FILE *fp);
extern int mat_parse(void);
extern void mat_restart(FILE *fp);

FaceArg::FaceArg(int v, int vn, int vt) :
    v(v), vn(vn), vt(vt)
{
    if((v != vn || vn != vt) && !has_warned_mismatch) {
        fputs("Warning: Element Mismatch, model will look bad\n", stderr);
        has_warned_mismatch = true;
    }
}

ModelParserState::ModelParserState() :
    ptr(NULL), current_group(NULL), current_mgroup(NULL)
{ }
ModelParserState::~ModelParserState() { }

void ModelParserState::attach(const char *fname, Model &model) {
    filename = fname;
    libname = "";
    size_t pos = filename.rfind("/");
    basename = std::string(filename, 0, pos);
    printf("basename = %s\n", basename.c_str());
    
    ptr = &model;
    current_group = &(model.getGroup(""));
    current_mgroup = &(current_group->getMatGroup(""));
    materials = &(model.materials);
    valid_mtl = negative_id = tesselations = false;
    current_mat = Material::Default;
    has_warned_mismatch = false;
}

void ModelParserState::vertex(GLfloat coords[3]) {
    PUSH_TRIPLE(ptr->vertices, coords);
    if(coords[0] < (ptr->min_x)) {
        ptr->min_x = coords[0];
    }
    if(coords[0] > (ptr->max_x)) {
        ptr->max_x = coords[0];
    }
    if(coords[1] < (ptr->min_y)) {
        ptr->min_y = coords[1];
    }
    if(coords[1] > (ptr->max_y)) {
        ptr->max_y = coords[1];
    }
    if(coords[2] < (ptr->min_z)) {
        ptr->min_z = coords[2];
    }
    if(coords[2] > (ptr->max_z)) {
        ptr->max_z = coords[2];
    }
}
void ModelParserState::normal(GLfloat coords[3]) {
    PUSH_TRIPLE(ptr->normals, coords);
}
void ModelParserState::texture(GLfloat coords[3]) {
    PUSH_TRIPLE(ptr->texture, coords);
}

int ModelParserState::resolve(int ind) {
    return (ind < 0 ? ptr->vertices.size() + ind : ind - 1);
}
void ModelParserState::face() {
    /* Empty stack, turn N points into a triangle fan. */
    if(face_args.size() <= 2) {
        printf("Too few arguments for face\n");
        face_args.clear();
        return;
    }
    int v[3] = {
        resolve(face_args[0].v),
        resolve(face_args[1].v),
        resolve(face_args[2].v)
    };
    PUSH_TRIPLE(current_mgroup->elements, v);
    
    for(size_t i = 3; i < face_args.size(); ++i) {
        v[1] = v[2];
        v[2] = resolve(face_args[i].v);
        PUSH_TRIPLE(current_mgroup->elements, v);
    }
    face_args.clear();
}

void ModelParserState::face_arg(int indices[3]) {
    face_args.push_back(FaceArg(indices[0], indices[1], indices[2]));
}
void ModelParserState::mtllib(const char *libname) {
}
void ModelParserState::usemtl(const char *mtlname) {
}
void ModelParserState::group(const char *groupname) {
    current_group = &(ptr->getGroup(groupname));
    /*TODO: bind next_mtl to current_group group IF set */
}

void ModelParserState::newmtl(const char *mtlname) {
    if(valid_mtl) {
        (*materials)[mtl_name] = current_mat;
    }
    current_mat = Material();
    mtl_name = mtlname;
    valid_mtl = true;
}

void ModelParserState::ka(GLfloat color[3]) {
    if(!valid_mtl) {
        fputs("Warning: ka before material definition.\n", stderr);
    }
    COPY_TRIPLE(color, current_mat.ka);
}
void ModelParserState::kd(GLfloat color[3]) {
    if(!valid_mtl) {
        fputs("Warning: kd before material definition.\n", stderr);
    }
    COPY_TRIPLE(color, current_mat.kd);
}
void ModelParserState::ks(GLfloat color[3]) {
    if(!valid_mtl) {
        fputs("Warning: ks before material definition.\n", stderr);
    }
    COPY_TRIPLE(color, current_mat.ks);
}
void ModelParserState::ns(GLfloat amount) {
    if(!valid_mtl) {
        fputs("Warning: ns before material definition.\n", stderr);
    }
    current_mat.ns = amount;
}
void ModelParserState::endmtlfile() {
    if(valid_mtl) {
        (*materials)[mtl_name] = current_mat;
    }
    valid_mtl = false;
}

void ModelParserState::stats() {
}

Model * ModelIO::Load(const char *fname) {
    Model *model = NULL;
    FILE *fp = fopen(fname, "r");
    if(!fp) {
        return NULL;
    }
    
    model = ModelIO::Load(fp, fname);
    
    fclose(fp);
    return model;
}

Model * ModelIO::Load(FILE *fp, const char *fname) {
    Model *model = new Model();
    
    model_parser_state.attach(fname, *model);
    wf_restart(fp);
    wf_parse();
    model->trim();
    
    model_parser_state.stats();
    return model;
}
