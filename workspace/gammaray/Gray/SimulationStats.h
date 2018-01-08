#ifndef simulation_stats_h
#define simulation_stats_h
#include <Daq/DaqStats.h>
#include <Gray/GammaRayTraceStats.h>

struct SimulationStats {
    GammaRayTraceStats physics;
    DaqStats daq;

    SimulationStats operator+=(const SimulationStats& rhs) {
        physics += rhs.physics;
        daq += rhs.daq;
        return (*this);
    }

    SimulationStats operator+(const SimulationStats& rhs) {
        SimulationStats result(*this);
        result += rhs;
        return (result);
    }
};
#endif // simulation_stats_h
