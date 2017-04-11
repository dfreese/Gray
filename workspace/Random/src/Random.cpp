#include <cmath>
#include <Random/Random.h>
#include <VrMath/LinearR3.h>

std::mt19937 Random::generator;
std::normal_distribution<double> Random::normal_distribution;
std::uniform_real_distribution<double> Random::uniform_distribution;
bool Random::seed_set = false;
unsigned long Random::seed_used = 0;

double Random::Uniform() {
    return(uniform_distribution(generator));
}

double Random::Gaussian() {
    return(normal_distribution(generator));
}

double Random::Exponential(const double lambda)
{
    std::exponential_distribution<double> exponential_distribution(lambda);
    return(exponential_distribution(generator));
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
    generator.seed(seed);
    seed_set = true;
    seed_used = seed;
}

void Random::Seed()
{
    unsigned long seed = std::time(NULL);
    Seed(seed);
}

unsigned long Random::GetSeed() {
    return(seed_used);
}

bool Random::SeedSet() {
    return(seed_set);
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


long Random::Poisson(double lambda)
{
    std::poisson_distribution<long> poisson_distribution(lambda);
    return(poisson_distribution(generator));
}
