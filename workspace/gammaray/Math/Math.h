#ifndef MATH_H
#define MATH_H

#include <vector>

namespace Math {
    double interpolate(const std::vector<double> & x,
                       const std::vector<double> & y,
                       double x_value);
}

#endif // MATH_H
