#include <Random.h>
#include <mt19937.h>
#include <cmath>
#include <LinearR3.h>

double Random::cache_gauss = 0;
bool Random::cache_valid = false;

double Random::Uniform() {
    return(genrand_real1());
}

/*!
 * Box-Mueller Transform to produce Gaussian from UniformSphere
 */
double Random::Gaussian() {
    if (cache_valid) {
        cache_valid = false;
        return(cache_gauss);
    }
    double x1, x2, w;

    do {
        x1 = 2.0 * Random::Uniform() - 1.0;
        x2 = 2.0 * Random::Uniform() - 1.0;
        w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0f );

    w = std::sqrt( (-2.0 * std::log( w ) ) / w );
    double y1 = x1 * w;
    cache_gauss = x2 * w;
    cache_valid = true;

    return y1;
}
double Random::Exponential(const double lambda)
{
    double s;
    if (Uniform() > 0.5) {
        s = 1.0;
    } else {
        s = -1.0;
    }
    return(-1.0 * s * std::log(1.0 - Random::Uniform()) / lambda);
}
void Random::RandomHemiCube(VectorR3 & p)
{
    double z1,z2;
    z1 = Random::Uniform();
    z2 = Random::Uniform();
    p.x = sqrt(z1 * (2.0 - z1)) * cos(2.0 * M_PI * z2);
    p.y = sqrt(z1 * (2.0 - z1)) * sin(2.0 * M_PI * z2);
    p.z = 1 - z1;
}
void Random::Seed(unsigned long seed)
{
    init_genrand(seed);
}
void Random::UniformSphere(VectorR3 & p)
{
    double cost = 2.0 * Random::Uniform() - 1.0;
    double phi = M_2_PI * Random::Uniform();
    double sint = std::sqrt(1.0 - cost * cost);
    p.x = sint * std::cos(phi);
    p.y = sint * std::sin(phi);
    p.z = cost;
    p.Normalize();
}
void Random::Acolinearity(const VectorR3 & b, VectorR3 &r, double radians)
{
    // Theta: angle relative to other photon, mean around 180.0 degrees
    // Phi: Angle around central axis any of 360 degrees in a circle
    double phi = M_2_PI * Random::Uniform();
    double theta = M_PI;

    // Generate acolinearity angle away from blue photon
    RotationMapR3 rotation;

    // Create rotation axis this is perpendicular to Y axis
    // using cross product and rotate red direction
    VectorR3 rot_axis = b;
    VectorR3 UnitY;
    UnitY.SetUnitY();
    rot_axis *= UnitY;
    rot_axis.Normalize();

    double g1 = Random::Gaussian();

    // blur theta by acolinearity
    theta += (g1 * radians);
    //theta += radians;

    // rotate red theta degrees away from blue
    r = b;
    rotation.Set(rot_axis, theta);
    rotation.Transform(&r);

    // using original blue axis rotate red around blue by 360 degrees
    rotation.Set(b,phi);
    rotation.Transform(&r);
}
