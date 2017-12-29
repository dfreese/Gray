#include <Daq/FilterFunctors.h>

namespace FilterFunctors {
FilterEnergyGateLow::FilterEnergyGateLow(double energy_val) :
    value(energy_val)
{
}

bool FilterEnergyGateLow::operator() (EventT & event) const {
    bool val = event.energy >= value;
    if (!val) {
        event.dropped = true;
    }
    return(val);
}

FilterEnergyGateHigh::FilterEnergyGateHigh(double energy_val) :
    value(energy_val)
{
}

bool FilterEnergyGateHigh::operator() (EventT & event) const {
    bool val = event.energy <= value;
    if (!val) {
        event.dropped = true;
    }
    return(val);
}
}
