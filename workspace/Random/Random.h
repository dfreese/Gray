// Random.h
//
//   A general purpose dynamically resizable sorted Stack
//   Implemented with templates.
//	Items are stored contiguously, for quick accessing.
//	Templated class must implement a < operator
//	Top of queue has smallest element
//
//
// Author: Peter Olcott.
// Contact: pdo@stanford.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.

#ifndef RANDOM_H
#define RANDOM_H
#define FWHM_to_sigma ((double)0.644391971)

class VectorR3;

class Random
{
public:
    static double Uniform();
    static double Gaussian();
    static double Exponential(const double lambda);
    static void RandomHemiCube(VectorR3 & p);
    static void Seed(unsigned long seed);
    static void UniformSphere(VectorR3 & p);
    static void Acolinearity(const VectorR3 & b, VectorR3 &r, double radians);
private:
    static double cache_gauss;
    static bool cache_valid;
};

#endif /* RANDOM_H */
