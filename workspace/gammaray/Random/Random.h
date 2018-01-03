// Random.h
//

#ifndef RANDOM_H
#define RANDOM_H

#include <random>
class VectorR3;

class Random
{
public:
    static void SeedDefault();
    static void SetSeed(unsigned long seed);
    static unsigned long GetSeed();
    static unsigned long Int();
    static double Uniform();
    static double Gaussian();
    static double Exponential(const double lambda);
    static long Poisson(double lambda);
    static VectorR3 UniformSphere();
    static VectorR3 UniformSphereFilled();
    static VectorR3 Deflection(const VectorR3 & ref, const double costheta);
    static VectorR3 Acolinearity(const VectorR3 & ref, double radians);
    static VectorR3 UniformCylinder(double height, double radius);
    static VectorR3 UniformAnnulusCylinder(double height, double radius);
    static VectorR3 UniformRectangle(const VectorR3 & size);
    static double GaussianEnergyBlur(double energy, double eres);
    static double GaussianEnergyBlurInverseSqrt(double energy, double eres,
                                                double ref_energy);
    static double GaussianBlurTime(double time, double tres);
    static double GaussianBlurTimeTrunc(double time, double tres,
                                        double max_blur);
    static bool Selection(double probability);
    static double LevinDoubleExp(double c, double k1, double k2);
    static double TruncatedLevinDoubleExp(double c, double k1, double k2,
                                           double max);
    static double TruncatedGaussian(double sigma, double max);
private:
    static std::mt19937 & generator();
    static std::normal_distribution<double> & normal_distribution();
    static unsigned long & seed_used();
};

#endif /* RANDOM_H */
