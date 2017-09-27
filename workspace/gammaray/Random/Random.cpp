#include <cmath>
#include <ctime>
#include <Random/Random.h>
#include <Random/Transform.h>
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

void Random::RankReseed(int rank) {
    std::uniform_int_distribution<unsigned int> dist;
    if (rank > 0) {
        generator.discard(rank);
        unsigned long new_seed = dist(generator);
        Seed(new_seed);
    }
}

VectorR3 Random::UniformSphere()
{
    return (Transform::UniformSphere(Random::Uniform(), Random::Uniform()));
}

VectorR3 Random::UniformSphereFilled()
{
    return (Transform::UniformSphereFilled(Random::Uniform(),
                                           Random::Uniform(),
                                           Random::Uniform()));
}

VectorR3 Random::Acolinearity(const VectorR3 & ref, double radians)
{
    return (Transform::Acolinearity(ref, radians, Random::Uniform(),
                                    Random::Gaussian()));
}

VectorR3 Random::UniformCylinder(double height, double radius) {
    return (Transform::UniformCylinder(height, radius,
                                       Random::Uniform(),
                                       Random::Uniform(),
                                       Random::Uniform()));
}

VectorR3 Random::UniformAnnulusCylinder(double height, double radius) {
    return (Transform::UniformAnnulusCylinder(height, radius,
                                              Random::Uniform(),
                                              Random::Uniform()));
}

long Random::Poisson(double lambda)
{
    std::poisson_distribution<long> poisson_distribution(lambda);
    return(poisson_distribution(generator));
}
