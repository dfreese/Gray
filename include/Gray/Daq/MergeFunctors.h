/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef MergeFunctors_h
#define MergeFunctors_h
#include <vector>
#include "Gray/Daq/Process.h"

namespace MergeFunctors {
    using EventT = Process::EventT;
    using DetIdT = Process::DetIdT;

    struct MergeFirst {
        void operator() (EventT & e0, EventT & e1) const;
    };

    struct MergeMax {
        void operator() (EventT & e0, EventT & e1) const;
    };

    struct MergeAnger {
        MergeAnger(const std::vector<DetIdT> & base,
                   const std::vector<DetIdT> & bx,
                   const std::vector<DetIdT> & by,
                   const std::vector<DetIdT> & bz);

        std::vector<DetIdT> create_reverse_map() const;
        int index(int blk, int bx, int by, int bz) const;
        void operator() (EventT & e0, EventT & e1) const;

        const std::vector<DetIdT> base;
        const std::vector<DetIdT> bx;
        const std::vector<DetIdT> by;
        const std::vector<DetIdT> bz;
        const int no_blk;
        const int no_bx;
        const int no_by;
        const int no_bz;
        const std::vector<DetIdT> reverse_map;
    };
}
#endif // MergeFunctors_h
