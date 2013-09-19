
#ifndef CS354_GENERIC_GEOMETRY_HPP
#define CS354_GENERIC_GEOMETRY_HPP

#include "../common.hpp"

namespace cs354 {
    struct Element {
        Element(int v, int vt, int vn);
        Element(int args[3]);
        ~Element();
        
        Element & operator=(const Element &rhs);
        
        bool operator==(const Element &rhs) const;
        /* Ordering, not used to denote anything of geometric significance */
        bool operator<(const Element &rhs) const;
        bool operator>(const Element &rhs) const;
        bool operator<=(const Element &rhs) const;
        bool operator>=(const Element &rhs) const;
        
        int v, vt, vn;
    };
    struct Triangle {
        Element v1, v2, v3;
    };
    
    template <typename T> struct Point;
    
    /* Template types, because why not make them generic? */
    template <typename T> struct Vector {
        Vector(T vx, T vy, T vz) :
            vx(vx), vy(vy), vz(vz)
        { }
        Vector(T args[3]) :
            vx(args[0]), vy(args[1]), vz(args[2])
        { }
        ~Vector() { }
        
        Vector<T> operator+(const Vector<T> &rhs) const {
            return Vector<T>(vx + rhs.vx, vy + rhs.vy, vz + rhs.vz);
        }
        Vector<T> & operator+=(const Vector<T> &rhs) {
            vx += rhs.vx;
            vy += rhs.vy;
            vz += rhs.vz;
            return (*this);
        }
        Point<T> operator+(const Point<T> &rhs) const {
            return Point<T>(rhs.x + vx, rhs.y + vy, rhs.z + vz);
        }
        Vector<T> operator-(const Vector<T> &rhs) const {
            return Vector<T>(vx - rhs.vx, vy - rhs.vy, vz - rhs.vz);
        }
        Vector<T> & operator-=(const Vector<T> &rhs) {
            vx -= rhs.vx;
            vy -= rhs.vy;
            vz -= rhs.vz;
            return (*this);
        }
        Vector<T> operator*(T &scalar) const {
            return Vector<T>(vx * scalar, vy * scalar, vz * scalar);
        }
        Vector<T> & operator*=(T &scalar) {
            vx *= scalar;
            vy *= scalar;
            vz *= scalar;
            return (*this);
        }
        Vector<T> & operator=(const Vector<T> &rhs) {
            vx = rhs.vx;
            vy = rhs.vy;
            vz = rhs.vz;
            return (*this);
        }
        
        Point<T> toPoint() {
            return Point<T>(vx, vy, vz);
        }
        
        T vx, vy, vz;
    };
    
    template <typename T> struct Point {
        Point(T x, T y, T z) :
            x(x), y(y), z(z)
        { }
        Point(T args[3]) :
            x(args[0]), y(args[1]), z(args[2])
        { }
        ~Point() { }
        
        Vector<T> operator-(const Point<T> &rhs) {
            return Vector<T>(x - rhs.x, y - rhs.y, z - rhs.z);
        }
        
        Point<T> operator+(const Vector<T> &rhs) {
            return Point<T>(x + rhs.vx, y + rhs.vy, z + rhs.vz);
        }
        Point<T> & operator+=(const Vector<T> &rhs) {
            x += rhs.vx;
            y += rhs.vy;
            z += rhs.vz;
            return *this;
        }
        
        Vector<T> toVector() {
            return Vector<T>(x, y, z);
        }
        
        T x, y, z;
    };
    
    typedef Point<GLfloat> Vertex;
    typedef Point<GLfloat> TextureCoord;
    typedef Vector<GLfloat> Normal;
    
}

#endif
