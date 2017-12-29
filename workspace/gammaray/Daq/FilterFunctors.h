#include <Daq/Process.h>

namespace FilterFunctors {
    using EventT = Process::EventT;
    using DetIdT = Process::DetIdT;

    struct FilterEnergyGateLow {
        FilterEnergyGateLow(double energy_val);
        bool operator() (EventT & event) const;
        const double value;
    };

    struct FilterEnergyGateHigh {
        FilterEnergyGateHigh(double energy_val);
        bool operator() (EventT & event) const;
        const double value;
    };

}
