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

/*!
 * Transforms a normal gaussian random variable and a uniform random variable
 * to model 511keV photon acolinearity in PET.  The photons are launched 180
 * degrees from each other, with some minor deviation in an arbitrary direction.
 * We calculate this by generating a copy of the reference vector, rotating
 * along an easy to calculate perpendicular axis, and then spinning the copy
 * around the reference with a [0, 2 * pi] uniform random variable.
 *
 * \param ref a unit direction vector
 * \param std_radians The standard deviation by which to scale the gaussian
 * \param deflection_dir_rand_uniform a uniform random variable [0,1] that
 * is translated into arbitrary axis along which the deflection is made.  This
 * is uniform 360 degrees around the reference vector.
 * \param deflection_angle_gaussian_rand a normal gaussian random variable that
 * is multiplied with std_radians to generate the gaussian deflection from ref.
 */
VectorR3 Transform::Acolinearity(const VectorR3 & ref,
                                 double std_radians,
                                 double deflection_dir_rand_uniform,
                                 double deflection_angle_gaussian_rand)
{
    // Phi: Angle around central axis any of 360 degrees in a circle
    const double phi = 2 * M_PI * deflection_dir_rand_uniform;

    // Create rotation axis perpendicular to ref by crossing ref with the y
    // unit vector, unless ref is the y unit vector, then just use unit x.
    const VectorR3 unit_y(0, 1, 0);
    const VectorR3 unit_x(1, 0, 0);
    const VectorR3 rot_axis = (ref == unit_y) ? unit_x:(ref * unit_y).Normalize();

    // Generate a gaussian with std of std_radians
    const double theta = (deflection_angle_gaussian_rand * std_radians);

    // Start with the reference
    VectorR3 ret(ref);

    // Generate acolinearity angle away from the reference photon using our
    // rotation map
    RotationMapR3 rotation;
    rotation.Set(rot_axis, theta);
    rotation.Transform(&ret);

    // using original blue axis rotate red around blue by phi
    rotation.Set(ref, phi);
    rotation.Transform(&ret);

    // Negate the vector to send it (roughly) off in the opposite direction
    ret.Negate();
    return (ret);
}

/*!
 * Transforms two uniform random variables into a annulus (empty) cylinder with
 * a particular height and radius.  The circle of the cylinder is in the XY
 * plane and the height is in Z.  The cylinder is centered at the origin.
 *
 * \param height the height of the cylinder in z
 * \param radius the radius of the cylinder int the xy plane
 * \param phi_rand_uniform a uniform random variable [0,1] that is transformed
 * into a random radian around the circle. x=cos(theta), y=sin(theta).
 * \param height_rand_uniform a uniform random variable [0,1] that dictates
 * where along the height [-height/2, height/2] the point will be placed.
 */
VectorR3 Transform::UniformAnnulusCylinder(double height, double radius,
                                           double theta_rand_uniform,
                                           double height_rand_uniform)
{
    const double theta = 2.0 * M_PI * theta_rand_uniform;
    const double x = radius * cos(theta);
    const double y = radius * sin(theta);
    const double z = height * (height_rand_uniform - 0.5);
    return (VectorR3(x, y, z));
}
