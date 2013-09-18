
#include "generic/Material.hpp"

#include "generic/Shader.hpp"

using namespace cs354;

Material Material::Default = {
    {0.5,0.5,0.5},
    {1.0,1.0,1.0},
    {1.0,1.0,1.0},
    1.0, 10.0,
    1,
    0,0,0,0,
    0,0
};

Material & Material::operator=(const Material &rhs) {
    ka[0] = rhs.ka[0];
    ka[1] = rhs.ka[1];
    ka[2] = rhs.ka[2];
    
    kd[0] = rhs.kd[0];
    kd[1] = rhs.kd[1];
    kd[2] = rhs.kd[2];
    
    ks[0] = rhs.ks[0];
    ks[1] = rhs.ks[1];
    ks[2] = rhs.ks[2];
    
    tr = rhs.tr;
    ns = rhs.ns;
    illum = rhs.illum;
    map_ka = rhs.map_ka;
    map_kd = rhs.map_kd;
    map_ks = rhs.map_ks;
    map_d = rhs.map_d;
    decal = rhs.decal;
    bump = rhs.bump;
    return (*this);
}

void Material::bind() const {
    if(MaterialLocations::Bound()) {
        glUniform3f(MaterialLocations::Ka(), ka[0], ka[1], ka[2]);
        glUniform3f(MaterialLocations::Kd(), kd[0], kd[1], kd[2]);
        glUniform3f(MaterialLocations::Ks(), ks[0], ks[1], ks[2]);
        glUniform1f(MaterialLocations::Tr(), tr);
        glUniform1f(MaterialLocations::Ns(), ns);
    }else {
        glColor3f(ka[0], kd[1], kd[2]);
    }
}

const MaterialLocations _default_locations;
/* MaterialLocations */
/* Static Interface */
const MaterialLocations *
MaterialLocations::current_locations = &_default_locations;


bool MaterialLocations::Bound() {
    return current_locations != &(_default_locations);
}

int MaterialLocations::Ka() {
    return current_locations->loc_ka;
}
int MaterialLocations::Kd() {
    return current_locations->loc_kd;
}
int MaterialLocations::Ks() {
    return current_locations->loc_ks;
}
int MaterialLocations::Tr() {
    return current_locations->loc_tr;
}
int MaterialLocations::Ns() {
    return current_locations->loc_ns;
}

void MaterialLocations::Bind(const Shader &shader) {
    current_locations = &(shader.getLocations());
}
void MaterialLocations::Unbind() {
    current_locations = &_default_locations;
}

/* Non-static Interface */

MaterialLocations::MaterialLocations() :
    loc_ka(-1),
    loc_kd(-1),
    loc_ks(-1),
    loc_tr(-1),
    loc_ns(-1)
{ }
MaterialLocations::~MaterialLocations() { }
