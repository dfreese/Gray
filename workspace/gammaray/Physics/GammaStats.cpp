/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Physics/GammaStats.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <Math/Math.h>
#include <Physics/Interaction.h>
#include <Physics/Physics.h>
#include <Random/Random.h>

using namespace std;

GammaStats::GammaStats() :
    index(-1),
    enable_interactions(true),
    log_material(false),
    // Make the form factor always 1
    compton_scatter({0.0, 1.0}, {1.0, 1.0}),
    rayleigh_scatter({0.0, 1.0}, {1.0, 1.0}),
    cache_len({-1, 0, 0, 0})
{
}

GammaStats::GammaStats(
    const std::string & name, int index,
    double density, bool sensitive, std::vector<double> energy,
    std::vector<double> matten_comp, std::vector<double> matten_phot,
    std::vector<double> matten_rayl, std::vector<double> x,
    std::vector<double> form_factor, std::vector<double> scattering_func) :
        name(name),
        index(index),
        energy(energy),
        photoelectric(matten_phot),
        compton(matten_comp),
        rayleigh(matten_rayl),
        log_energy(energy.size()),
        log_photoelectric(matten_phot.size()),
        log_compton(matten_comp.size()),
        log_rayleigh(matten_rayl.size()),
        x(x),
        form_factor(form_factor),
        scattering_func(scattering_func),
        enable_interactions(true),
        log_material(sensitive),
        compton_scatter(x, scattering_func),
        rayleigh_scatter(x, form_factor),
        cache_len({-1, 0, 0, 0})
{
    // Convert the mass attenuation coefficient to a linear attenuation
    // coefficient by multiplying by density.
    std::transform(photoelectric.begin(), photoelectric.end(),
                   photoelectric.begin(),
                   std::bind1st(std::multiplies<double>(),density));
    std::transform(compton.begin(), compton.end(),
                   compton.begin(),
                   std::bind1st(std::multiplies<double>(),density));
    std::transform(rayleigh.begin(), rayleigh.end(),
                   rayleigh.begin(),
                   std::bind1st(std::multiplies<double>(),density));

    // Cache the log values for log interpolation
    auto log_func = [](double & val) { return (std::log(val)); };
    std::transform(energy.begin(), energy.end(),
                   log_energy.begin(), log_func);
    std::transform(photoelectric.begin(), photoelectric.end(),
                   log_photoelectric.begin(), log_func);
    std::transform(compton.begin(), compton.end(),
                   log_compton.begin(), log_func);
    std::transform(rayleigh.begin(), rayleigh.end(),
                   log_rayleigh.begin(), log_func);
}

void GammaStats::SetName(const std::string & n)
{
    name = n;
}

void GammaStats::SetMaterialType(int s)
{
    index = s;
}

void GammaStats::SetFileName(const std::string & n)
{
    filename = n;
    name = n;
}

int GammaStats::GetMaterial() const
{
    return index;
}

bool GammaStats::LogMaterial() const {
    return (log_material);
}

void GammaStats::SetLogMaterial(bool val) {
    log_material = val;
}

bool GammaStats::InteractionsEnabled() const {
    return (enable_interactions);
}

void GammaStats::DisableInteractions()
{
    enable_interactions = false;
}

bool GammaStats::Load()
{
    ifstream infile(filename);
    if (!infile) {
        cerr << " Unable to open file: " << filename << endl;
        return false;
    }
    string line;
    getline(infile, line);
    stringstream line_stream(line);
    int no_points;
    if ((line_stream >> no_points).fail()) {
        return(false);
    }
    if (0 >= no_points) {
        return(false);
    }

    energy.clear();
    energy.resize(no_points, -1);
    photoelectric.clear();
    photoelectric.resize(no_points, -1);
    compton.clear();
    compton.resize(no_points, -1);
    rayleigh.clear();
    rayleigh.resize(no_points, -1);

    for (int i = 0; i < no_points; i++) {
        string pt_line;
        getline(infile, pt_line);
        stringstream pt_stream(pt_line);
        bool line_fail = false;
        line_fail |= (pt_stream >> energy[i]).fail();
        line_fail |= (pt_stream >> photoelectric[i]).fail();
        line_fail |= (pt_stream >> compton[i]).fail();
        line_fail |= (pt_stream >> rayleigh[i]).fail();

        if (line_fail) {
            cerr << "Error reading file: " << filename << " on line: "
                 << (i + 1) << endl;
            return(false);
        }
    }

    log_energy.resize(no_points);
    log_photoelectric.resize(no_points);
    log_compton.resize(no_points);
    log_rayleigh.resize(no_points);

    auto log_func = [](double & val) { return (std::log(val)); };
    std::transform(energy.begin(), energy.end(),
                   log_energy.begin(), log_func);
    std::transform(photoelectric.begin(), photoelectric.end(),
                   log_photoelectric.begin(), log_func);
    std::transform(compton.begin(), compton.end(),
                   log_compton.begin(), log_func);
    std::transform(rayleigh.begin(), rayleigh.end(),
                   log_rayleigh.begin(), log_func);
    return(true);
}

GammaStats::AttenLengths GammaStats::GetAttenLengths(double e) const {
    std::lock_guard<std::mutex> lock(cache_lock);
    if (e != cache_len.energy) {
        size_t idx = Math::interp_index(energy, e);
        const double log_e = std::log(e);
        cache_len.energy = e;
        cache_len.photoelectric =
            std::exp(Math::interpolate(log_energy, log_photoelectric, log_e, idx));
        cache_len.compton =
            std::exp(Math::interpolate(log_energy, log_compton, log_e, idx));
        cache_len.rayleigh =
            std::exp(Math::interpolate(log_energy, log_rayleigh, log_e, idx));
    }
    return (cache_len);
}

void GammaStats::DisableRayleigh() {
    log_rayleigh = std::vector<double>(rayleigh.size(), std::log(0));
    rayleigh = std::vector<double>(rayleigh.size(), 0);
}

void GammaStats::ComptonScatter(Photon& p) const {
    const double costheta = compton_scatter.scatter_angle(p.GetEnergy(), Random::Uniform());
    // After collision the photon loses some energy to the electron
    p.SetEnergy(Physics::KleinNishinaEnergy(p.GetEnergy(), costheta));
    p.SetDir(Random::Deflection(p.GetDir(),costheta));
    p.SetScatterCompton();
}

void GammaStats::RayleighScatter(Photon& p) const {
    const double costheta = rayleigh_scatter.scatter_angle(p.GetEnergy(), Random::Uniform());
    p.SetDir(Random::Deflection(p.GetDir(), costheta));
    // If the photon scatters on a non-detector, it is a scatter, checked
    // inside SetScatter
    p.SetScatterRayleigh();
}

bool GammaStats::Distance(Photon& photon, double max_dist) const {
    if (!InteractionsEnabled()) {
        // move photon to interaction point, or exit point of material
        photon.AddPos(max_dist * photon.GetDir());
        photon.AddTime(max_dist * Physics::inverse_speed_of_light);
        return (false);
    }

    AttenLengths len = GetAttenLengths(photon.GetEnergy());
    double rand_dist = Random::Exponential(len.total());
    if (rand_dist > max_dist) {
        // move photon to the exit point of material
        photon.AddPos(max_dist * photon.GetDir());
        photon.AddTime(max_dist * Physics::inverse_speed_of_light);
        return (false);
    }

    // move the photon to the interaction point
    photon.AddPos(rand_dist * photon.GetDir());
    photon.AddTime(rand_dist * Physics::inverse_speed_of_light);
    return (true);
}

Interaction::Type GammaStats::Interact(Photon& photon) const {
    AttenLengths len = GetAttenLengths(photon.GetEnergy());
    double rand = len.total() * Random::Uniform();
    if (rand <= len.photoelectric) {
        photon.SetEnergy(0);
        return (Interaction::Type::PHOTOELECTRIC);
    } else if (rand <= (len.photoelectric + len.compton)) {
        // perform compton kinematics
        ComptonScatter(photon);
        return (Interaction::Type::COMPTON);
    } else {
        // perform rayleigh kinematics
        RayleighScatter(photon);
        return (Interaction::Type::RAYLEIGH);
    }
}
