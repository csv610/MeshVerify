#ifndef PREDICATES_HPP
#define PREDICATES_HPP

#include "Mesh.hpp"

class Predicates {
public:
    /**
     * @brief Initialize robust predicates (precomputes necessary constants).
     */
    static void init();

    /**
     * @brief Robust 2D orientation test.
     * @return Positive if CCW, negative if CW, zero if collinear.
     */
    static double orient2d(const Point& a, const Point& b, const Point& c);

    /**
     * @brief Robust 2D in-circle test.
     * @return Positive if d is inside circumcircle of abc (if abc is CCW), 
     *         negative if outside, zero if on.
     */
    static double incircle(const Point& a, const Point& b, const Point& c, const Point& d);

    /**
     * @brief Robust 3D orientation test.
     * @return Positive if CCW, negative if CW, zero if coplanar.
     */
    static double orient3d(const Point3D& a, const Point3D& b, const Point3D& c, const Point3D& d);

    /**
     * @brief Robust 3D in-sphere test.
     * @return Positive if e is inside circumsphere of abcd (if abcd is CCW), 
     *         negative if outside, zero if on.
     */
    static double insphere(const Point3D& a, const Point3D& b, const Point3D& c, const Point3D& d, const Point3D& e);

private:
    // Helper to calculate determinant sign robustly
    // This is a simplified robust implementation using long double 
    // for increased precision, which handles most near-degenerate cases.
    // For 100% exactness, adaptive precision arithmetic (Shewchuk) is required.
    template<typename T>
    static T det3x3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22);
};

#endif // PREDICATES_HPP
