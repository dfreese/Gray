#include "Transform.h"
#include <VrMath/LinearR3.h>
#include <cmath>

/*!
 * From two uniform random variables [0,1], construct a VectorR3 uniformly
 * distributed across the unit sphere.
 *
 * See (6, 7, and 8) at http://mathworld.wolfram.com/SpherePointPicking.html
 *
 * \param a uniform random variable [0,1] representing the angle theta
 * \param a uniform random variable [0,1] representing the cos phi.
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
 * Interprets a uniform random variable as the cube of the radius to scale an
 * output of UniformSphere so that the interior of a sphere is uniformly
 * covered by points without using a rejection algorithm.
 *
 * \param a uniform random variable [0,1] that represents the cube of the
 * radius of the point.
 */
VectorR3 Transform::UniformSphereFilled(double theta_rand_uniform,
                                        double cos_phi_rand_uniform,
                                        double r_cubed_rand_uniform)
{
    const double radius_scale = std::pow(r_cubed_rand_uniform, 1.0/3);
    return (radius_scale * UniformSphere(theta_rand_uniform, cos_phi_rand_uniform));
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

    // Create rotation axis perpendicular
    const VectorR3 rot_axis = GetOrtho(ref);

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
 * Transforms two uniform random variables into a uniform cylinder with
 * a particular height and radius.  The circle of the cylinder is in the XY
 * plane and the height is in Z.  The cylinder is centered at the origin.
 *
 * To avoid rejection testing, this formulation is used:
 * http://mathworld.wolfram.com/DiskPointPicking.html
 *
 * A height of 0 produces points uniformly distributed on the unit disk, in this
 * case the height_random_uniform variable can also be a constant.
 *
 * \param height the height of the cylinder in z
 * \param radius the radius of the cylinder int the xy plane
 * \param theta_rand_uniform a uniform random variable [0,1] that is transformed
 * into a random radian around the circle. x=cos(theta), y=sin(theta).
 * \param radius_squared_rand_uniform a uniform random varaible [0,1] that is
 * treated as the square of the radius, square-rooted to scale the radius
 * appropriately.
 * \param height_rand_uniform a uniform random variable [0,1] that dictates
 * where along the height [-height/2, height/2] the point will be placed.
 */
VectorR3 Transform::UniformCylinder(double height, double radius,
                                    double height_rand_uniform,
                                    double radius_squared_rand_uniform,
                                    double theta_rand_uniform)
{
    const double theta = 2.0 * M_PI * theta_rand_uniform;
    const double radius_rand = radius * std::sqrt(radius_squared_rand_uniform);
    const double x = radius_rand * cos(theta);
    const double y = radius_rand * sin(theta);
    const double z = height * (height_rand_uniform - 0.5);
    return (VectorR3(x, y, z));
}

/*!
 * Transforms two uniform random variables into a annulus (empty) cylinder with
 * a particular height and radius.  The circle of the cylinder is in the XY
 * plane and the height is in Z.  The cylinder is centered at the origin.
 *
 * \param height the height of the cylinder in z
 * \param radius the radius of the cylinder int the xy plane
 * \param theta_rand_uniform a uniform random variable [0,1] that is transformed
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

/*!
 * Transforms three uniform random variables into a point uniformly distributed
 * in a rectangle with a specified size.  The rectangle is centered around the
 * origin.
 *
 * \param size the size of the rectangle.  [-size.[x,y,z] / 2, size.[x,y,z] / 2]
 * \param rand_x uniform random variable [0,1] translated into an x position,
 * zero being most negative.
 * \param rand_y uniform random variable [0,1] translated into an y position
 * \param rand_z uniform random variable [0,1] translated into an z position
 */
VectorR3 Transform::UniformRectangle(const VectorR3 & size,
                                     double rand_x, double rand_y,
                                     double rand_z)
{
    const VectorR3 pos((rand_x - 0.5) * size.x,
                       (rand_y - 0.5) * size.y,
                       (rand_z - 0.5) * size.z);
    return (pos);
}

/*!
 * Blurs an energy by a percentage full-width at half-max (FWHM).  Takes a
 * normal gaussian random variable and converts it into an appropriate
 * percentage blur.  The resulting energy is:
 *
 * E = E * (1 + N(eres / 2.35))
 *
 * \param energy The current energy of the event.  Non-negative required.
 * \param eres the energy blur to apply as a ratio. (1.0 = 100%, 0.13 = 13%)
 * Non-negative required.
 * \param blur_rand_gauss a normally distributed gaussian random variable
 */
double Transform::GaussianEnergyBlur(double energy, double eres,
                                     double blur_rand_gauss)
{
    return (energy * (1.0 + eres * fwhm_to_sigma * blur_rand_gauss));
}

/*!
 * Blurs an energy using GaussianEnergyBlur, but first scales the energy
 * resolution by:
 *
 * Eres = Eres * (sqrt(reference) / sqrt(energy))
 *
 * Eres is left unchanged if energy is zero.
 * \param ref_energy The reference energy from which to calculate the
 * degredation.
 */
double Transform::GaussianEnergyBlurInverseSqrt(double energy, double eres,
                                                double ref_energy,
                                                double blur_rand_gauss)
{
    // Make sure to handle the divide by zero case so we don't inject nan or
    // inf into energy.
    eres *= (energy > 0) ? std::sqrt(ref_energy) / std::sqrt(energy):1.0;
    return (GaussianEnergyBlur(energy, eres, blur_rand_gauss));
}

/*!
 * Generate a time blurred by a gaussian with a FWHM in the same units.
 *
 * \param time the unblurred time of the event
 * \param tres the FWHM of the gaussian to be used.  Must be in the same units
 * as time and non-negative.
 * \param blur_rand_gauss a normally distributed gaussian random variable
 */
double Transform::GaussianBlurTime(double time, double tres,
                                   double blur_rand_gauss)
{
    return (time + tres * fwhm_to_sigma * blur_rand_gauss);
}
