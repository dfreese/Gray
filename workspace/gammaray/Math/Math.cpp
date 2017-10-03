#include <Math/Math.h>

#include <vector>

/*!
 * For a given set of x and y, perform piecewise linear interpolation between
 * the y values for a specified x.  If x_value is greater than the last value
 * in x, then the last value in y is returned.  If x_value is less than the
 * first value, then the first value in y is returned.
 *
 * \param x a monotonically increasing vector of values to interpolate within.
 * \param y the values of off of which the interpolation is done.  Must be the
 * same size as x.
 *
 */
double Math::interpolate(const std::vector<double> & x,
                         const std::vector<double> & y,
                         double x_value)
{
    size_t idx = upper_bound(x.begin(), x.end(), x_value) - x.begin();

    if (idx == 0) {
        return(y.front());
    } else if (idx == x.size()) {
        return(y.back());
    }
    double delta = x[idx] - x[idx - 1];
    double alpha = (x_value - x[idx - 1]) / delta;
    if (alpha > 1.0) {
        alpha = 1.0;
    }
    double dsigma_max = ((1.0 - alpha) * y[idx - 1] + alpha * y[idx]);
    return(dsigma_max);
}
