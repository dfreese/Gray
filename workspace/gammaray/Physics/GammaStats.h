/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef GAMMA_STATS_H
#define GAMMA_STATS_H

#include <mutex>
#include <string>
#include <vector>
#include <Physics/Compton.h>
#include <Physics/Interaction.h>
#include <Physics/Photon.h>
#include <Physics/Physics.h>
#include <Physics/Rayleigh.h>

class GammaStats
{
public:

    GammaStats();
    GammaStats(const std::string & name, int index,
               double density, bool sensitive, std::vector<double> energy,
               std::vector<double> matten_comp, std::vector<double> matten_phot,
               std::vector<double> matten_rayl, std::vector<double> x,
               std::vector<double> form_factor,
               std::vector<double> scattering_func);
    bool Load();
    int GetId() const;
    bool IsSensitive() const;
    bool InteractionsEnabled() const;
    void DisableInteractions();
    void DisableRayleigh();
    void ComptonScatter(Photon& p) const;
    void RayleighScatter(Photon& p) const;
    bool Distance(Photon& photon, double max_dist) const;
    Interaction::Type Interact(Photon& photon) const;

private:
    struct AttenLengths {
        double energy;
        double photoelectric;
        double compton;
        double rayleigh;
        double total() const {
            return (photoelectric + compton + rayleigh);
        }
    };
    AttenLengths GetAttenLengths(double energy) const;

    std::string name;
    int index;
    std::string filename;
    std::vector<double> energy;
    std::vector<double> photoelectric;
    std::vector<double> compton;
    std::vector<double> rayleigh;
    std::vector<double> log_energy;
    std::vector<double> log_photoelectric;
    std::vector<double> log_compton;
    std::vector<double> log_rayleigh;
    const std::vector<double> x;
    const std::vector<double> form_factor;
    const std::vector<double> scattering_func;

    bool enable_interactions;
    bool sensitive;

    Compton compton_scatter;
    Rayleigh rayleigh_scatter;

    mutable std::mutex cache_lock;
    mutable AttenLengths cache_len;
};

#endif
