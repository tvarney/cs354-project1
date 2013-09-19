
#include "generic/Geometry.hpp"

using namespace cs354;

Element::Element(int v, int vt, int vn) :
    v(v), vt(vt), vn(vn)
{ }
Element::Element(int args[3]) :
    v(args[0]), vt(args[1]), vn(args[2])
{ }
Element::~Element() { }

Element & Element::operator=(const Element &rhs) {
    v = rhs.v;
    vt = rhs.vt;
    vn = rhs.vn;
    return *this;
}

bool Element::operator==(const Element &rhs) const {
    return (rhs.v == v && rhs.vt == vt && rhs.vn == vn);
}
bool Element::operator<(const Element &rhs) const {
    return ((v != rhs.v && v < rhs.v) ||
            (vt != rhs.vt && vt < rhs.vt) ||
            (vn != rhs.vn && vn < rhs.vn));
}
bool Element::operator>(const Element &rhs) const {
    return ((v != rhs.v && v > rhs.v) ||
            (vt != rhs.vt && vt > rhs.vt) ||
            (vn != rhs.vn && vn > rhs.vn));
}
bool Element::operator<=(const Element &rhs) const {
    return ((*this) == rhs || (*this) < rhs);
}
bool Element::operator>=(const Element &rhs) const {
    return ((*this) == rhs || (*this) > rhs);
}
