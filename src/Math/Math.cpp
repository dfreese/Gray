/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Math/Math.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

/*!
 * Returns the index to be used for a interpolate call.
 * Useful if multiple interp calls will be made and the index should be saved.
 */
size_t Math::interp_index(const std::vector<double>& x, double x_value) {
    return (std::upper_bound(x.begin(), x.end(), x_value) - x.begin());
}

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
    size_t idx = std::upper_bound(x.begin(), x.end(), x_value) - x.begin();

    if (idx == 0) {
        return(y.front());
    } else if (idx == x.size()) {
        return(y.back());
    }
    double delta = x[idx] - x[idx - 1];
    double alpha = (x_value - x[idx - 1]) / delta;
    if (alpha >= 1.0) {
        // This is required, because if we end up with y[idx-1] == -inf, then
        // this will return -nan. Reverse for positive. If we use this for
        // log-log interpolation, then we need to handle that case appropriately.
        return (y[idx]);
    }
    double dsigma_max = ((1.0 - alpha) * y[idx - 1] + alpha * y[idx]);
    return(dsigma_max);
}

double Math::interpolate(const std::vector<double> & x,
                         const std::vector<double> & y,
                         double x_value, size_t idx)
{
    if (idx == 0) {
        return(y.front());
    } else if (idx == x.size()) {
        return(y.back());
    } else if (x_value >= x[idx]) {
        // Required to so that we don't have a 0 * y[idx] in case y[idx]
        // is inf intentionally.
        return (y[idx]);
    } else if (x_value == x[idx - 1]) {
        // Required to so that we don't have a 0 * y[idx - 1] in case y[idx-1]
        // is inf intentionally.
        return (y[idx - 1]);
    }

    double delta = x[idx] - x[idx - 1];
    double alpha = (x_value - x[idx - 1]) / delta;
    double dsigma_max = ((1.0 - alpha) * y[idx - 1] + alpha * y[idx]);
    return(dsigma_max);
}

double Math::loglog_interpolate(const std::vector<double> & x,
                                const std::vector<double> & y,
                                double x_value)
{
    size_t idx = std::upper_bound(x.begin(), x.end(), x_value) - x.begin();

    if (idx == 0) {
        return(y.front());
    } else if (idx == x.size()) {
        return(y.back());
    }
    const double e1 = x[idx - 1];
    const double e2 = x[idx];
    const double d1 = y[idx - 1];
    const double d2 = y[idx];

    double value = (std::log(d1) +
                    (std::log(d2 / d1)/std::log(e2 / e1) *
                     std::log(x_value / e1)));
    value = std::exp(value);
    return (value);
}

double Math::loglog_interpolate(const std::vector<double> & x,
                                const std::vector<double> & y,
                                double x_value, size_t idx)
{
    if (idx == 0) {
        return(y.front());
    } else if (idx == x.size()) {
        return(y.back());
    }
    const double e1 = x[idx - 1];
    const double e2 = x[idx];
    const double d1 = y[idx - 1];
    const double d2 = y[idx];

    double value = (std::log10(d1) +
                    (std::log10(d2 / d1)/std::log10(e2 / e1) *
                     std::log10(x_value / e1)));
    value = std::pow(10.,value);
    return (value);
}


/*!
 * For a given set of x, y, and z perform piecewise linear interpolation
 * between the z values for a specified x and y.  If x_value or y_value is
 * greater than the last value in x or y, respectively, then the last value in
 * z in that respective direction is returned.  If x_value or y_value is less
 * than the first value, then the first value of z in the respective direction
 * is returned.
 *
 * \param x a monotonically increasing vector of values to interpolate within.
 * \param y a monotonically increasing vector of values to interpolate within.
 * \param z the values of off of which the interpolation is done.  Must be the
 * size [x.size(), y.size()].
 *
 */
double Math::interpolate_2d(const std::vector<double> & x,
                            const std::vector<double> & y,
                            const std::vector<std::vector<double>> & z,
                            double x_value, double y_value)
{
    size_t x_idx = std::upper_bound(x.begin(), x.end(), x_value) - x.begin();
    size_t y_idx = std::upper_bound(y.begin(), y.end(), y_value) - y.begin();

    x_idx = std::min(std::max(x_idx, 1ul), x.size() - 1);
    y_idx = std::min(std::max(y_idx, 1ul), y.size() - 1);

    const double delta_x = x[x_idx] - x[x_idx - 1];
    const double delta_y = y[y_idx] - y[y_idx - 1];

    double alpha_x = (x_value - x[x_idx - 1]) / delta_x;
    double alpha_y = (y_value - y[y_idx - 1]) / delta_y;

    alpha_x = std::min(std::max(0.0, alpha_x), 1.0);
    alpha_y = std::min(std::max(0.0, alpha_y), 1.0);

    double val = (alpha_x * alpha_y * z[x_idx][y_idx] +
                  alpha_x * (1.0 - alpha_y) * z[x_idx][y_idx - 1] +
                  (1.0 - alpha_x) * alpha_y * z[x_idx - 1][y_idx] +
                  (1.0 - alpha_x) * (1.0 - alpha_y) * z[x_idx - 1][y_idx - 1]);
    return (val);
}

/*!
 * Assuming a smoothly varying and monotonically increasing in y function, this
 * interpolates the appropriate y_value.
 *
 * \param x a monotonically increasing vector of values to interpolate within.
 * \param y a monotonically increasing vector of values to interpolate within.
 * \param z the values of off of which the interpolation is done.  Must be the
 * size [x.size(), y.size()].
 */
double Math::interpolate_y_2d(const std::vector<double> & x,
                              const std::vector<double> & y,
                              const std::vector<std::vector<double>> & z,
                              double x_value, double z_value)
{
    size_t x_idx = std::upper_bound(x.begin(), x.end(), x_value) - x.begin();
    x_idx = std::min(std::max(x_idx, 1ul), x.size() - 1);

    const double delta_x = x[x_idx] - x[x_idx - 1];
    double alpha_x = (x_value - x[x_idx - 1]) / delta_x;

    alpha_x = std::min(std::max(0.0, alpha_x), 1.0);

    const double y_val0 = interpolate(z[x_idx - 1], y, z_value);
    const double y_val1 = interpolate(z[x_idx], y, z_value);

    const double val = (1.0 - alpha_x) * y_val0 + alpha_x * y_val1;
    return (val);
}

/*!
 * Returns a vector no_points in length with the values evenly spaced on
 * [start, end].  Note the start and end value are included in the range.
 */
std::vector<double> Math::linspace(double start, double end, int no_points)
{
    // Minus one on the number of points because we include the end point.
    double step = (end - start) / (no_points - 1);
    std::vector<double> ret(no_points);
    int n = 0;
    for (double & ret_val: ret) {
        ret_val = start + n * step;
        n++;
    }
    return (ret);
}

/*!
 *
 */
std::vector<double> Math::cos_space(int no_points)
{
    std::vector<double> ret(linspace(0, M_PI, no_points));
    std::transform(ret.begin(), ret.end(), ret.begin(),
                   [](double theta) { return (std::cos(theta)); });
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

/*!
 * An integer mixing hash for unsigned longs taken from here:
 * https://stackoverflow.com/a/12996028/2465202
 */
unsigned long Math::hash(unsigned long x) {
    constexpr unsigned long mult1(0xbf58476d1ce4e5b9);
    constexpr unsigned long mult2(0x94d049bb133111eb);
    x = (x ^ (x >> 30)) * mult1;
    x = (x ^ (x >> 27)) * mult2;
    x = x ^ (x >> 31);
    return x;
}
