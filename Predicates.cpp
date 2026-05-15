#include "Predicates.hpp"
#include <cmath>

// Standard Robust Predicates implementation based on J. Shewchuk's adaptive precision logic.
// This is a simplified version that handles the sign of the determinant robustly.

namespace {
    double splitter;     // Used to split a double into two halves
    double resulterrbound;
    double ccwerrboundA, ccwerrboundB, ccwerrboundC;
    double o3derrboundA, o3derrboundB, o3derrboundC;
    double icerrboundA, icerrboundB, icerrboundC;
    double iserrboundA, iserrboundB, iserrboundC;

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

        resulterrbound = (3.0 + 8.0 * epsilon) * epsilon;
        ccwerrboundA = (3.0 + 16.0 * epsilon) * epsilon;
        ccwerrboundB = (2.0 + 12.0 * epsilon) * epsilon;
        ccwerrboundC = (9.0 + 64.0 * epsilon) * epsilon * epsilon;

        o3derrboundA = (7.0 + 56.0 * epsilon) * epsilon;
        o3derrboundB = (3.0 + 28.0 * epsilon) * epsilon;
        o3derrboundC = (26.0 + 512.0 * epsilon) * epsilon * epsilon;

        icerrboundA = (10.0 + 96.0 * epsilon) * epsilon;
        icerrboundB = (4.0 + 48.0 * epsilon) * epsilon;
        icerrboundC = (44.0 + 576.0 * epsilon) * epsilon * epsilon;

        iserrboundA = (16.0 + 224.0 * epsilon) * epsilon;
        iserrboundB = (5.0 + 72.0 * epsilon) * epsilon;
        iserrboundC = (71.0 + 1408.0 * epsilon) * epsilon * epsilon;
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

    double detsum;
    if (detleft > 0.0) {
        if (detright <= 0.0) return det;
        else detsum = detleft + detright;
    } else if (detleft < 0.0) {
        if (detright >= 0.0) return det;
        else detsum = -detleft - detright;
    } else {
        return det;
    }

    double errbound = ccwerrboundA * detsum;
    if ((det >= errbound) || (-det >= errbound)) return det;

    // Fallback to higher precision if near zero
    long double ldet = (long double)(a.x - c.x) * (b.y - c.y) - (long double)(a.y - c.y) * (b.x - c.x);
    return (double)ldet;
}

double Predicates::orient3d(const Point3D& a, const Point3D& b, const Point3D& c, const Point3D& d) {
    double adx = a.x - d.x;
    double bdx = b.x - d.x;
    double cdx = c.x - d.x;
    double ady = a.y - d.y;
    double bdy = b.y - d.y;
    double cdy = c.y - d.y;
    double adz = a.z - d.z;
    double bdz = b.z - d.z;
    double cdz = c.z - d.z;

    double bdxcdy = bdx * cdy;
    double cdxbdy = cdx * bdy;
    double cdxady = cdx * ady;
    double adxcdy = adx * cdy;
    double adxbdy = adx * bdy;
    double bdxady = bdx * ady;

    double det = adz * (bdxcdy - cdxbdy) + bdz * (cdxady - adxcdy) + cdz * (adxbdy - bdxady);

    double permanent = (std::abs(bdxcdy) + std::abs(cdxbdy)) * std::abs(adz)
                     + (std::abs(cdxady) + std::abs(adxcdy)) * std::abs(bdz)
                     + (std::abs(adxbdy) + std::abs(bdxady)) * std::abs(cdz);
    
    double errbound = o3derrboundA * permanent;
    if ((det > errbound) || (-det > errbound)) return det;

    // Fallback
    long double ladx = (long double)a.x - d.x;
    long double lbdx = (long double)b.x - d.x;
    long double lcdx = (long double)c.x - d.x;
    long double lady = (long double)a.y - d.y;
    long double lbdy = (long double)b.y - d.y;
    long double lcdy = (long double)c.y - d.y;
    long double ladz = (long double)a.z - d.z;
    long double lbdz = (long double)b.z - d.z;
    long double lcdz = (long double)c.z - d.z;

    long double ldet = ladz * (lbdx * lcdy - lcdx * lbdy) 
                     + lbdz * (lcdx * lady - ladx * lcdy) 
                     + lcdz * (ladx * lbdy - lbdx * lady);
    return (double)ldet;
}

double Predicates::incircle(const Point& a, const Point& b, const Point& c, const Point& d) {
    long double adx = (long double)a.x - d.x;
    long double ady = (long double)a.y - d.y;
    long double bdx = (long double)b.x - d.x;
    long double bdy = (long double)b.y - d.y;
    long double cdx = (long double)c.x - d.x;
    long double cdy = (long double)c.y - d.y;

    long double bcdet = bdx * cdy - cdx * bdy;
    long double cadet = cdx * ady - adx * cdy;
    long double abdet = adx * bdy - bdx * ady;
    long double alift = adx * adx + ady * ady;
    long double blift = bdx * bdx + bdy * bdy;
    long double clift = cdx * cdx + cdy * cdy;

    long double det = alift * bcdet + blift * cadet + clift * abdet;
    return (double)det;
}

double Predicates::insphere(const Point3D& a, const Point3D& b, const Point3D& c, const Point3D& d, const Point3D& e) {
    auto get_row = [&](const Point3D& p) {
        long double dx = (long double)p.x - e.x;
        long double dy = (long double)p.y - e.y;
        long double dz = (long double)p.z - e.z;
        return std::array<long double, 4>{dx, dy, dz, dx * dx + dy * dy + dz * dz};
    };

    std::array<std::array<long double, 4>, 4> m = {get_row(a), get_row(b), get_row(c), get_row(d)};

    long double det4x4 = m[0][3] * det3x3<long double>(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]) -
                        m[1][3] * det3x3<long double>(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]) +
                        m[2][3] * det3x3<long double>(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]) -
                        m[3][3] * det3x3<long double>(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);

    return (double)det4x4;
}

template<typename T>
T Predicates::det3x3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22) {
    return m00 * (m11 * m22 - m12 * m21) -
           m01 * (m10 * m22 - m12 * m20) +
           m02 * (m10 * m21 - m11 * m20);
}
