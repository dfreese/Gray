#include <Math/Math.h>

#include <vector>
#include <numeric>

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

/*!
 * Returns a vector no_points in length with the values evenly spaced on
 * [start, end].  Note the start and end value are included in the range.
 */
std::vector<double> Math::linspace(double start, double end, int no_points)
{
    // Minus one on the number of points because we include the end point.
    double step = (end - start) / (no_points - 1);
    double val = start;
    std::vector<double> ret(no_points);
    for (double & ret_val: ret) {
        ret_val = val;
        val += step;
    }
    return (ret);
}

/*!
 * Performs the trapezoidal approximation to an integral on x and y.  It
 * assumes x is monotonically increasing, that x and y are two points or
 * longer, and x and y are the same size.
 */
std::vector<double> Math::trap_z(const std::vector<double> & x,
                                 const std::vector<double> & y)
{
    std::vector<double> result(y.size());
    result.front() = 0;
    for (size_t ii = 1; ii < x.size(); ++ii) {
        result[ii] = result[ii - 1];
        result[ii] += (y[ii] + y[ii - 1]) / 2 * (x[ii] - x[ii - 1]);
    }
    return (result);
}

/*!
 * Converts a discrete sampling of a pdf into a cdf using the trapezodial
 * integral approximation, and then normalizing the final integral to one.  A
 * strict pdf that integrates to one is not required, unnormalized
 * cross-sections are allowed.  The same requirements for x and y of trap_z are
 * required of x and pdf, with the addition that pdf must be nonnegative.
 */
std::vector<double> Math::pdf_to_cdf(const std::vector<double> & x,
                                     const std::vector<double> & pdf)
{
    std::vector<double> cdf(Math::trap_z(x, pdf));
    // Make sure we are correctly normalized to one.
    for (double & val: cdf) {
        val /= cdf.back();
    }
    return (cdf);
}
