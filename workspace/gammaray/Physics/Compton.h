#ifndef COMPTON_H
#define COMPTON_H

#include <vector>

class Compton {
public:
    Compton(const std::vector<double>& x,
            const std::vector<double>& scattering_func);
    static double x_val(double cos_theta, double energy_mev);
    static std::vector<double> x_val(
            const std::vector<double>& costheta,
            const double energy_mev);
    static std::vector<double> scattering(
            const std::vector<double>& costheta,
            const double energy_mev,
            const std::vector<double>& x,
            const std::vector<double>& scattering_func);
    static std::vector<double> dsigma(
            const std::vector<double>& costheta,
            const double energy_mev,
            const std::vector<double>& x,
            const std::vector<double>& scattering_func);
    static std::vector<std::vector<double>> create_scatter_cdfs(
            const std::vector<double>& energies,
            const std::vector<double>& costhetas,
            const std::vector<double>& x,
            const std::vector<double>& scattering_func);
    double scatter_angle(double energy, double rand_uniform) const;
private:
    std::vector<double> energy_idx;
    std::vector<double> costheta_idx;
    std::vector<std::vector<double>> scatter_cdfs;
};

#endif // COMPTON_H
