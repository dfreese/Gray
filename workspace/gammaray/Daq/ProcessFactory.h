/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef ProcessFactory_H
#define ProcessFactory_H
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <Daq/Mapping.h>
#include <Daq/Process.h>
#include <Daq/BlurProcess.h>
#include <Daq/FilterProcess.h>
#include <Daq/MergeProcess.h>

namespace ProcessFactory {
    using EventT = Process::EventT;
    using TimeT = Process::TimeT;
    using DetIdT = Process::DetIdT;
    using IdLookupT = Mapping::IdLookupT;
    using IdMappingT = Mapping::IdMappingT;

    struct ProcessDescription {
        std::string type;
        std::string subtype;
        std::vector<std::string> args;
        bool as_double(size_t idx, double & val) const {
            try {
                val = std::stod(args.at(idx));
                return (true);
            } catch (...) {
                return (false);
            }
        }
        bool as_time(size_t idx, TimeT & val) const {
            if (idx >= args.size()) {
                return (false);
            }
            std::stringstream ss(args[idx]);
            return (!(ss >> val).fail());
        }
    };
    int ProcessDescLine(const std::string & line, ProcessDescription & desc);
    int ProcessDescLines(
            const std::vector<std::string> & lines,
            std::vector<ProcessDescription> & process_descriptions);
    int ProcessDescStream(
            std::istream& input,
            std::vector<ProcessDescription> & process_descriptions);
    int ProcessDescFile(
            const std::string& filename,
            std::vector<ProcessDescription> & process_descriptions);

    std::unique_ptr<Process> BlurFactory(BlurProcess::BlurF filt_func);
    std::unique_ptr<Process> BlurFactory(ProcessDescription desc);
    std::unique_ptr<Process> DeadtimeFactory(TimeT value, bool paralyzable,
                                             const IdLookupT& id_map);
    std::unique_ptr<Process> DeadtimeFactory(ProcessDescription desc,
                                             const IdMappingT& id_maps);

    std::unique_ptr<Process> CoincFactory(ProcessDescription desc);
    std::unique_ptr<Process> CoincFactory(TimeT value,
                                          bool reject_multiples,
                                          bool paralyzable,
                                          TimeT window_offset);

    std::unique_ptr<Process> FilterFactory(FilterProcess::FilterF filt_func);
    std::unique_ptr<Process> FilterFactory(ProcessDescription desc);
    std::unique_ptr<Process> MergeFactory(MergeProcess::MergeF merge_func,
                                          TimeT value,
                                          const IdLookupT& id_map);
    std::unique_ptr<Process> MergeFactory(
            ProcessDescription desc,
            const std::map<std::string, std::vector<DetIdT>> & id_maps);
    std::unique_ptr<Process> SortFactory(TimeT value);

    std::unique_ptr<Process> ProcessFactory(
            ProcessDescription desc,
            const std::map<std::string, std::vector<DetIdT>> & id_maps);
    constexpr double default_max_time_blur = 3.0;
}

#endif // ProcessFactory_H
