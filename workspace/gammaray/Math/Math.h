#ifndef MATH_H
#define MATH_H

#include <cstddef>
#include <vector>

namespace Math {
    double interpolate(const std::vector<double> & x,
                       const std::vector<double> & y,
                       double x_value);
    double interpolate(const std::vector<double> & x,
                       const std::vector<double> & y,
                       double x_value, size_t idx);
    double loglog_interpolate(const std::vector<double> & x,
                              const std::vector<double> & y,
                              double x_value);
    double loglog_interpolate(const std::vector<double> & x,
                              const std::vector<double> & y,
                              double x_value, size_t idx);
    double interpolate_2d(const std::vector<double> & x,
                          const std::vector<double> & y,
                          const std::vector<std::vector<double>> & z,
                          double x_value, double y_value);
    double interpolate_y_2d(const std::vector<double> & x,
                            const std::vector<double> & y,
                            const std::vector<std::vector<double>> & z,
                            double x_value, double z_value);
    std::vector<double> linspace(double start, double end, int no_points);
    std::vector<double> cos_space(int no_points);
    std::vector<double> trap_z(const std::vector<double> & x,
                               const std::vector<double> & y);
    std::vector<double> pdf_to_cdf(const std::vector<double> & x,
                                   const std::vector<double> & pdf);
    unsigned long hash(unsigned long x);
}

#endif // MATH_H
