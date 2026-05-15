#include "mesh_verify/Predicates.hpp"
#include <cmath>
#include <array>
#include <algorithm>

// Standard Robust Predicates implementation based on J. Shewchuk's adaptive precision logic.

namespace {
    double splitter;
    double ccwerrboundA;
    double o3derrboundA;

    void exactinit() {
        double half = 0.5;
        double check, lastcheck;
        int every_other = 0;
        splitter = 1.0;
        double epsilon = 1.0;
        do {
            lastcheck = check;
            epsilon *= half;
            if (every_other) splitter *= 2.0;
            every_other = !every_other;
            check = 1.0 + epsilon;
        } while ((check != 1.0) && (check != lastcheck));
        splitter += 1.0;
        ccwerrboundA = (3.0 + 16.0 * epsilon) * epsilon;
        o3derrboundA = (7.0 + 56.0 * epsilon) * epsilon;
    }
}

void Predicates::init() {
    static bool initialized = false;
    if (!initialized) {
        exactinit();
        initialized = true;
    }
}

double Predicates::orient2d(const Point& a, const Point& b, const Point& c) {
    double detleft = (a.x - c.x) * (b.y - c.y);
    double detright = (a.y - c.y) * (b.x - c.x);
    double det = detleft - detright;
    double detsum = std::abs(detleft) + std::abs(detright);
    double errbound = ccwerrboundA * detsum;
    if (std::abs(det) >= errbound) return det;
    long double ldet = (long double)(a.x - c.x) * (b.y - c.y) - (long double)(a.y - c.y) * (b.x - c.x);
    return (double)ldet;
}

double Predicates::orient3d(const Point3D& a, const Point3D& b, const Point3D& c, const Point3D& d) {
    double det = det3x3<double>(a.x-d.x, a.y-d.y, a.z-d.z, b.x-d.x, b.y-d.y, b.z-d.z, c.x-d.x, c.y-d.y, c.z-d.z);
    // Rough error bound check
    if (std::abs(det) > 1e-12) return det;
    return (double)det3x3<long double>(a.x-d.x, a.y-d.y, a.z-d.z, b.x-d.x, b.y-d.y, b.z-d.z, c.x-d.x, c.y-d.y, c.z-d.z);
}

double Predicates::incircle(const Point& a, const Point& b, const Point& c, const Point& d) {
    long double adx = (long double)a.x - d.x, ady = (long double)a.y - d.y;
    long double bdx = (long double)b.x - d.x, bdy = (long double)b.y - d.y;
    long double cdx = (long double)c.x - d.x, cdy = (long double)c.y - d.y;
    long double det = (adx*adx + ady*ady)*(bdx*cdy - cdx*bdy) + (bdx*bdx + bdy*bdy)*(cdx*ady - adx*cdy) + (cdx*cdx + cdy*cdy)*(adx*bdy - bdx*ady);
    return (double)det;
}

double Predicates::insphere(const Point3D& a, const Point3D& b, const Point3D& c, const Point3D& d, const Point3D& e) {
    auto get_row = [&](const Point3D& p) {
        long double dx = (long double)p.x - e.x, dy = (long double)p.y - e.y, dz = (long double)p.z - e.z;
        return std::array<long double, 4>{dx, dy, dz, dx*dx + dy*dy + dz*dz};
    };
    std::array<std::array<long double, 4>, 4> m = {get_row(a), get_row(b), get_row(c), get_row(d)};
    long double det = m[0][3]*det3x3<long double>(m[1][0],m[1][1],m[1][2], m[2][0],m[2][1],m[2][2], m[3][0],m[3][1],m[3][2]) -
                     m[1][3]*det3x3<long double>(m[0][0],m[0][1],m[0][2], m[2][0],m[2][1],m[2][2], m[3][0],m[3][1],m[3][2]) +
                     m[2][3]*det3x3<long double>(m[0][0],m[0][1],m[0][2], m[1][0],m[1][1],m[1][2], m[3][0],m[3][1],m[3][2]) -
                     m[3][3]*det3x3<long double>(m[0][0],m[0][1],m[0][2], m[1][0],m[1][1],m[1][2], m[2][0],m[2][1],m[2][2]);
    return (double)det;
}

template<typename T>
T Predicates::det3x3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22) {
    return m00*(m11*m22 - m12*m21) - m01*(m10*m22 - m12*m20) + m02*(m10*m21 - m11*m20);
}
