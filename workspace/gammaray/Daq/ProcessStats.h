#ifndef process_stats_h
#define process_stats_h
#include <sstream>
#include <string>

struct ProcessStats {
    long no_dropped = 0;
    long no_kept = 0;
    long no_coinc_pair_events = 0;
    long no_coinc_multiples_events = 0;
    long no_coinc_single_events = 0;
    long no_coinc_events = 0;

    long no_events() const {
        return (no_kept + no_dropped + no_coinc_pair_events +
                no_coinc_multiples_events + no_coinc_single_events);
    }

    std::string coinc_info() const {
        std::stringstream ss;
        ss << "coinc events            : " << no_coinc_events << "\n"
            << "events in coinc pair    : " << no_coinc_pair_events << "\n"
            << "events in coinc multiple: " << no_coinc_multiples_events << "\n"
            << "events in coinc single  : " << no_coinc_single_events << "\n";
        return (ss.str());
    }

    ProcessStats& operator+=(const ProcessStats& rhs) {
        no_dropped += rhs.no_dropped;
        no_kept += rhs.no_kept;
        no_coinc_pair_events += rhs.no_coinc_pair_events;
        no_coinc_multiples_events += rhs.no_coinc_multiples_events;
        no_coinc_single_events += rhs.no_coinc_single_events;
        no_coinc_events += rhs.no_coinc_events;
        return (*this);
    }

    ProcessStats operator+(const ProcessStats& rhs) {
        ProcessStats result(*this);
        result += rhs;
        return (result);
    }
};

#endif // process_stats_h
