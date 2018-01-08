#ifndef daq_stats_h
#define daq_stats_h

#include <ostream>
#include <vector>
#include <Daq/ProcessStats.h>

struct DaqStats {
    long no_events = 0;
    long no_kept = 0;
    long no_dropped = 0;
    long no_merged = 0;
    long no_filtered = 0;
    long no_deadtimed = 0;

    std::vector<ProcessStats> coinc_stats;
    std::vector<long> no_kept_per_proc;
    std::vector<long> no_dropped_per_proc;
    std::vector<bool> print_info;

    friend std::ostream& operator << (std::ostream& os, const DaqStats& s) {
        os << "events: " << s.no_events << "\n"
            << "kept: " << s.no_kept << "\n"
            << "dropped: " << s.no_dropped << "\n";
        os << "kept per level: ";
        for (size_t idx = 0; idx < s.print_info.size(); idx++) {
            if (s.print_info[idx]) {
                os << s.no_kept_per_proc[idx];
                if (idx + 1 < s.print_info.size()) {
                    os << ", ";
                }
            }
        }
        os << "\n";
        os << "drop per level: ";
        for (size_t idx = 0; idx < s.print_info.size(); idx++) {
            if (s.print_info[idx]) {
                os << s.no_dropped_per_proc[idx];
                if (idx + 1 < s.print_info.size()) {
                    os << ", ";
                }
            }
        }
        os << "\n";
        os << "merged: " << s.no_merged << "\n";
        os << "filtered: " << s.no_filtered << "\n";
        os << "deadtimed: " << s.no_deadtimed << "\n";
        for (auto& p: s.coinc_stats) {
            os << p.coinc_info();
        }
        return(os);
    }
};

#endif // daq_stats_h
