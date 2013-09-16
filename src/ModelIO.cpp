
/**
 * ModelIO:
 * Free functions for loading models from disc.
 * Author: Troy Varney - tav285 [troy.a.varney@gmail.com]
 */

#include "generic/ModelIO.hpp"
#include "generic/Model.hpp"
#include "generic/Parser.hpp"

#include <cstring>
#include <cstdio>
#include <climits>
#include <cstdlib>

using namespace cs354;

#define RuntimeException(msg) std::runtime_error(std::string(msg))

ModelParserState cs354::model_parser_state;

FaceArg::FaceArg(int v, int vn, int vt) :
    v(v), vn(vn), vt(vt)
{ }

ModelParserState::ModelParserState(Model *ptr) :
    ptr(ptr), current_group(NULL)
{
    if(ptr != NULL) {
        current_group = &(ptr->getGroup(""));
        current_mgroup = &(current_group->getMatGroup(""));
    }
}
ModelParserState::~ModelParserState() { }

void ModelParserState::attach(Model &model) {
    ptr = &model;
    current_group = &(model.getGroup(""));
}

void ModelParserState::vertex(GLfloat coords[3]) {
    ptr->vertices.push_back(coords[0]);
    ptr->vertices.push_back(coords[1]);
    ptr->vertices.push_back(coords[2]);
}
void ModelParserState::normal(GLfloat coords[3]) {
    ptr->normals.push_back(coords[0]);
    ptr->normals.push_back(coords[1]);
    ptr->normals.push_back(coords[2]);
}
void ModelParserState::texture(GLfloat coords[3]) {
    ptr->texture.push_back(coords[0]);
    ptr->texture.push_back(coords[1]);
    ptr->texture.push_back(coords[2]);
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
    int v1 = resolve(face_args[0].v);
    int v2 = resolve(face_args[1].v);
    int v3 = resolve(face_args[2].v);
    current_mgroup->elements.push_back(v1);
    current_mgroup->elements.push_back(v2);
    current_mgroup->elements.push_back(v3);
    
    for(size_t i = 3; i < face_args.size(); ++i) {
        v2 = v3;
        v3 = resolve(face_args[i].v);
        current_mgroup->elements.push_back(v1);
        current_mgroup->elements.push_back(v2);
        current_mgroup->elements.push_back(v3);
    }
    face_args.clear();
}

void ModelParserState::face_arg(int indices[3]) {
    face_args.push_back(FaceArg(indices[0], indices[1], indices[2]));
}
void ModelParserState::mtllib(const char *libname) {
    printf("Ignoring 'mtllib %s'\n", libname);
}
void ModelParserState::usemtl(const char *mtlname) {
    printf("Ignoring 'usemtl %s'\n", mtlname);
}
void ModelParserState::group(const char *groupname) {
    current_group = &(ptr->getGroup(groupname));
    /*TODO: bind next_mtl to current_group group IF set */
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

extern int wf_parse(void);
extern void wf_restart(FILE *fp);
Model * ModelIO::Load(FILE *fp, const char *fname) {
    Model *model = new Model();
    
    model_parser_state.attach(*model);
    wf_restart(fp);
    wf_parse();
    model->trim();
    return model;
}
