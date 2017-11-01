#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <VrMath/LinearR3.h>

namespace Transform {
    VectorR3 UniformSphere(double theta_rand_uniform,
                           double cos_phi_rand_uniform);
    VectorR3 UniformSphereFilled(double theta_rand_uniform,
                                 double cos_phi_rand_uniform,
                                 double r_cubed_rand_uniform);
    VectorR3 Deflection(const VectorR3 & ref,
                        const double costheta,
                        double deflection_dir_rand_uniform);
    VectorR3 Acolinearity(const VectorR3 & ref, double std_radians,
                          double deflection_dir_rand_uniform,
                          double deflection_angle_gaussian_rand);
    VectorR3 UniformCylinder(double height, double radius,
                             double height_rand_uniform,
                             double radius_rand_uniform,
                             double theta_rand_uniform);
    VectorR3 UniformAnnulusCylinder(double height, double radius,
                                    double theta_rand_uniform,
                                    double height_rand_uniform);
    VectorR3 UniformRectangle(const VectorR3 & size,
                              double rand_x, double rand_y, double rand_z);
    double GaussianEnergyBlur(double energy, double eres,
                              double blur_rand_gaus);
    double GaussianEnergyBlurInverseSqrt(double energy, double eres,
                                         double ref_energy,
                                         double blur_rand_gauss);
    double GaussianBlurTime(double time, double tres, double blur_rand_gauss);
    bool Selection(double probability, double test_rand_uniform);
    // 2.0 * std::sqrt(2.0 * std::log(2.0)) evaluated because cmath isn't
    // constexpr in c++11;
    constexpr double sigma_to_fwhm = 2.3548200450309493;
    constexpr double fwhm_to_sigma = 1.0 / sigma_to_fwhm;
}

#endif // TRANSFORM_H
