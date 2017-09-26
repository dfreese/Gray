#include "Transform.h"
#include <VrMath/LinearR3.h>
#include <cmath>

/*!
 * From two uniform random variables [0,1], construct a VectorR3 uniformly
 * distributed across the unit sphere.
 *
 * See (6, 7, and 8) at http://mathworld.wolfram.com/SpherePointPicking.html
 */
VectorR3 Transform::UniformSphere(double theta_rand_uniform,
                                  double cos_phi_rand_uniform)
{
    const double theta = 2.0 * M_PI * theta_rand_uniform;
    const double cos_phi = 2.0 * cos_phi_rand_uniform - 1.0;
    const double sin_phi = std::sqrt(1.0 - cos_phi * cos_phi);
    return (VectorR3(sin_phi * std::cos(theta),
                     sin_phi * std::sin(theta),
                     cos_phi));
}
