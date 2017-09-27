#include <VrMath/LinearR3.h>

namespace Transform {
    VectorR3 UniformSphere(double theta_rand_uniform,
                           double cos_phi_rand_uniform);
    VectorR3 Acolinearity(const VectorR3 & ref, double std_radians,
                          double deflection_dir_rand_uniform,
                          double deflection_angle_gaussian_rand);
}
