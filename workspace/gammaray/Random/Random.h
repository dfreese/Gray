// Random.h
//

#ifndef RANDOM_H
#define RANDOM_H

#include <random>
class VectorR3;

class Random
{
public:
    static void Seed(unsigned long seed);
    static bool SeedSet();
    static void Seed();
    static unsigned long GetSeed();
    static void RankReseed(int rank);
    static double Uniform();
    static double Gaussian();
    static double Exponential(const double lambda);
    static VectorR3 UniformSphere();
    static VectorR3 Acolinearity(const VectorR3 & ref, double radians);
    static long Poisson(double lambda);
    static VectorR3 UniformCylinder(double height, double radius);
    static VectorR3 UniformAnnulusCylinder(double height, double radius);
    
private:
    static std::mt19937 generator;
    static std::normal_distribution<double> normal_distribution;
    static std::uniform_real_distribution<double> uniform_distribution;
    static bool seed_set;
    static unsigned long seed_used;
};

#endif /* RANDOM_H */
