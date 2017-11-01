#include <cmath>
#include <ctime>
#include <Random/Random.h>
#include <Random/Transform.h>
#include <VrMath/LinearR3.h>

// There seems to be a bug particularly within the blurring functions when Gray
// is run on a 32bit system.  It's not worth trying to trace that down right now
// so until then, don't compile on 32bit systems, which will have an 4byte
// pointer compared to 8 on a 64bit system.
// TODO: implement tests that show this failure on 32bit
static_assert(sizeof(void*) == 8,
              "Only 64bit compilers are supported at this time");

std::mt19937 & Random::generator() {
    static std::mt19937 * generator = new std::mt19937();
    return (*generator);
}

/*!
 * Keep the normal distribution as a static member, rather than a local
 * variable as most implementations of normal distributions generate two
 * numbers, one which can be saved for later calls.
 */
std::normal_distribution<double> & Random::normal_distribution() {
    static auto * normal = new std::normal_distribution<double>();
    return (*normal);
}

bool & Random::seed_set() {
    static bool * seed_set = new bool(false);
    return (*seed_set);
}

unsigned long & Random::seed_used() {
    static auto * seed_used = new unsigned long(std::mt19937::default_seed);
    return (*seed_used);
}

unsigned long Random::Int() {
    return(generator()());
}

double Random::Uniform() {
    std::uniform_real_distribution<double> uniform;
    return(uniform(generator()));
}

double Random::Gaussian() {
    return(normal_distribution()(generator()));
}

double Random::Exponential(const double lambda)
{
    std::exponential_distribution<double> exponential_distribution(lambda);
    return(exponential_distribution(generator()));
}

long Random::Poisson(double lambda)
{
    std::poisson_distribution<long> poisson_distribution(lambda);
    return(poisson_distribution(generator()));
}

void Random::Seed(unsigned long seed)
{
    generator().seed(seed);
    seed_set() = true;
    seed_used() = seed;
}

void Random::Seed()
{
    unsigned long seed = std::time(NULL);
    Seed(seed);
}

void Random::SeedDefault() {
    Seed(std::mt19937::default_seed);
}

unsigned long Random::GetSeed() {
    return(seed_used());
}

bool Random::SeedSet() {
    return(seed_set());
}

void Random::RankReseed(int rank) {
    Seed(GetSeed() + rank);
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

/*!
 * Returns a vector deflected from the current direction at an angle of theta
 * radians.  The phi angle around the reference vector is uniformly sampled.
 */
VectorR3 Random::Deflection(const VectorR3 & ref, const double costheta)
{
    return (Transform::Deflection(ref, costheta, Random::Uniform()));
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

VectorR3 Random::UniformRectangle(const VectorR3 & size) {
    return (Transform::UniformRectangle(size, Random::Uniform(),
                                        Random::Uniform(),
                                        Random::Uniform()));
}

double Random::GaussianEnergyBlur(double energy, double eres) {
    return (Transform::GaussianEnergyBlur(energy, eres, Random::Gaussian()));
}

double Random::GaussianEnergyBlurInverseSqrt(double energy, double eres,
                                             double ref_energy)
{
    return (Transform::GaussianEnergyBlurInverseSqrt(energy, eres, ref_energy, Random::Gaussian()));
}

double Random::GaussianBlurTime(double time, double tres) {
    return (Transform::GaussianBlurTime(time, tres, Random::Gaussian()));
}

double Random::GaussianBlurTimeTrunc(double time, double tres,
                                     double max_blur)
{
    // TODO: implement truncated gaussian that doesn't rely on rejection
    double time_blur;
    do {
        time_blur = GaussianBlurTime(0, tres);
    } while(std::abs(time_blur) > max_blur);
    return (time_blur + time);
}

/*!
 * Generate a random boolean with a given probability of being true.
 */
bool Random::Selection(double probability) {
    if (probability <= 0) {
        return (false);
    } else if (probability >= 1) {
        return (true);
    } else {
        return (Transform::Selection(probability, Random::Uniform()));
    }
}

double Random::LevinDoubleExp(double c, double k1, double k2) {
    return (Random::Exponential(Random::Selection(c) ? k1:k2));
}

double Random::TruncatedLevinDoubleExp(double c, double k1, double k2,
                                       double max)
{
    double range;
    do {
        range = Random::LevinDoubleExp(c, k1, k2);
    } while (range > max);
    return(range);
}

double Random::TruncatedGaussian(double sigma, double max) {
    double range;
    do {
        range = Random::Gaussian() * sigma;
    } while (range > max);
    return(range);
}

