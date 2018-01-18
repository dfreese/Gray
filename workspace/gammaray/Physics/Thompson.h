#ifndef THOMPSON_H
#define THOMPSON_H

#include <vector>

class Thompson {
public:
    static double dsigma(const double costheta);
    static std::vector<double> dsigma(const std::vector<double>& costheta);
};

#endif // THOMPSON_H
