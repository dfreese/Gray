#ifndef MATH_H
#define MATH_H

#include <vector>

namespace Math {
    double interpolate(const std::vector<double> & x,
                       const std::vector<double> & y,
                       double x_value);
    std::vector<double> linspace(double start, double end, int no_points);
    std::vector<double> trap_z(const std::vector<double> & x,
                               const std::vector<double> & y);
    std::vector<double> pdf_to_cdf(const std::vector<double> & x,
                                   const std::vector<double> & pdf);
}

#endif // MATH_H
