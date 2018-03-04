/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Daq/ProcessFactory.h"
#include <fstream>
#include "Gray/Daq/CoincProcess.h"
#include "Gray/Daq/DeadtimeProcess.h"
#include "Gray/Daq/BlurFunctors.h"
#include "Gray/Daq/BlurProcess.h"
#include "Gray/Daq/FilterFunctors.h"
#include "Gray/Daq/FilterProcess.h"
#include "Gray/Daq/MergeFunctors.h"
#include "Gray/Daq/MergeProcess.h"
#include "Gray/Daq/SortProcess.h"
#include "Gray/Output/IO.h"

namespace ProcessFactory {

/*!
 * Turn a line in a file into a ProcessDescription.  0 indicates success.  Less
 * than zero an error.
 */
int ProcessDescLine(const std::string & line, ProcessDescription & desc) {
    std::stringstream line_ss(line);
    if ((line_ss >> desc.type).fail()) {
        return(-1);
    }

    std::string txt_val;
    while (line_ss >> txt_val) {
        desc.args.push_back(txt_val);
    }
    if (!desc.args.empty()) {
        desc.subtype = desc.args[0];
    }
    return(0);
}

int ProcessDescLines(
    const std::vector<std::string> & lines,
    std::vector<ProcessDescription> & process_descriptions)
{
    for (const auto & line: lines) {
        ProcessDescription proc_desc;
        int status = ProcessFactory::ProcessDescLine(line, proc_desc);
        if (status < 0) {
            return(status);
        }
        process_descriptions.push_back(proc_desc);
    }
    return(0);
}

int ProcessDescStream(
        std::istream& input,
        std::vector<ProcessDescription> & process_descriptions)
{
    if (!input) {
        return(-1);
    }
    std::vector<std::string> lines;
    std::string line;
    while (IO::GetLineCommentLeadWs(input, line)) {
        if (line.empty()) {
            continue;
        }
        lines.push_back(line);
    }
    return (ProcessDescLines(lines, process_descriptions));
}

int ProcessDescFile(
    const std::string& filename,
    std::vector<ProcessDescription> & process_descriptions)
{
    std::ifstream input(filename);
    return (ProcessDescStream(input, process_descriptions));
}

std::unique_ptr<Process> BlurFactory(BlurProcess::BlurF blur_func) {
    return (std::unique_ptr<Process>(new BlurProcess(blur_func)));
}

std::unique_ptr<Process> BlurFactory(ProcessDescription desc) {
    if (desc.args.size() < 2) {
        std::cerr << "blur format is: blur [type] [value] (options...)\n";
        return (nullptr);
    }
    BlurProcess::BlurF blur_func;
    if (desc.subtype == "energy") {
        double value;
        if (!desc.as_double(1, value)) {
            std::cerr << desc.args[1] << " is not a valid ratio\n";
            return (nullptr);
        }
        if (desc.args.size() == 2) {
            blur_func = BlurFunctors::BlurEnergy(value);
        } else if ((desc.args[2] == "at") && (desc.args.size() >= 4)) {
            double ref_energy;
            if (!desc.as_double(3, ref_energy)) {
                std::cerr << desc.args[3] << " is an invalid ref. energy\n";
                return (nullptr);
            }
            blur_func = BlurFunctors::BlurEnergyReferenced(value, ref_energy);
        } else {
            std::cerr << "unrecognized blur option: " << desc.args[2] << "\n";
            return(nullptr);
        }
    } else if (desc.subtype == "time") {
        TimeT value;
        if (!desc.as_time(1, value)) {
            std::cerr << desc.args[1] << " is not a valid time value\n";
            return (nullptr);
        }
        // Allow the value to be 3 FWHM on either side of the current event
        // TODO: allow this to be set by options.
        const TimeT max_blur = default_max_time_blur * value;
        blur_func = BlurFunctors::BlurTime(value, max_blur);
    } else {
        std::cerr << "Unknown blur type: " << desc.subtype << std::endl;
        return (nullptr);
    }
    return (BlurFactory(blur_func));
}

std::unique_ptr<Process> DeadtimeFactory(TimeT value, bool paralyzable,
                                         const IdLookupT& id_map)
{
    return (std::unique_ptr<Process>(new DeadtimeProcess(id_map, value,
                                                         paralyzable)));
}

std::unique_ptr<Process> DeadtimeFactory(
        ProcessDescription desc,
        const IdMappingT& id_maps)
{

    if (desc.args.size() < 2) {
        std::cerr << "deadtime format is: "
                  << "deadtime [component] [value] (options...)\n";
        return (nullptr);
    }
    const std::string & map_name = desc.args[0];
    TimeT value;
    if (!desc.as_double(1, value)) {
        std::cerr << desc.args[1] << " is not a valid deadtime value\n";
        return (nullptr);
    }

    bool paralyzable = false;
    for (size_t ii = 2; ii < desc.args.size(); ii++) {
        const std::string & option = desc.args[ii];
        if (option == "paralyzable") {
            paralyzable = true;
        } else if (option == "nonparalyzable") {
            paralyzable = false;
        } else {
            std::cerr << "unrecognized deadtime option: " << option << "\n";
            return(nullptr);
        }
    }
    auto map_iter = id_maps.find(map_name);
    if (map_iter == id_maps.end()) {
        std::cerr << "Unknown id map type: " << map_name << std::endl;
        return(nullptr);
    }
    const std::vector<DetIdT> id_map = (*map_iter).second;
    return (DeadtimeFactory(value, paralyzable, id_map));
}

std::unique_ptr<Process> CoincFactory(TimeT value, bool reject_multiples,
                                      bool paralyzable, TimeT window_offset)
{
    return (std::unique_ptr<Process>(new CoincProcess(value, reject_multiples,
                                                      paralyzable,
                                                      window_offset)));
}

std::unique_ptr<Process> CoincFactory(ProcessDescription desc) {
    if (desc.args.size() < 2) {
        std::cerr << "coinc format is: coinc [window/delay] [width]"
                  << " (options...)\n";
    }
    const std::string & name = desc.args[0];
    double value;
    if (!desc.as_double(1, value)) {
        std::cerr << desc.args[1] << " is not a valid value\n";
        return (nullptr);
    }

    bool paralyzable = false;
    bool reject_multiples = true;
    TimeT window_offset = 0;
    bool look_for_offset = false;
    if (name == "window") {
    } else if (name == "delay") {
        look_for_offset = true;
    } else {
        std::cerr << "Unknown coinc type: " << name << std::endl;
        return(nullptr);
    }
    size_t option_start = 2;
    if (look_for_offset) {
        option_start = 3;
        if (desc.args.size() < 3) {
            std::cerr << "no delay offset specified\n";
            return(nullptr);
        }
        if (!desc.as_double(2, window_offset)) {
            std::cerr << "invalid coinc delay offset: " << desc.args[2] << "\n";
            return (nullptr);
        }
    }
    for (size_t ii = option_start; ii < desc.args.size(); ii++) {
        const std::string & option = desc.args[ii];
        if (option == "keep_multiples") {
            reject_multiples = false;
        } else if (option == "paralyzable") {
            paralyzable = true;
        } else {
            std::cerr << "unrecognized coinc option: " << option << "\n";
            return(nullptr);
        }
    }
    return (CoincFactory(value, reject_multiples, paralyzable, window_offset));
}

std::unique_ptr<Process> FilterFactory(FilterProcess::FilterF filt_func) {
    return (std::unique_ptr<Process>(new FilterProcess(filt_func)));
}

std::unique_ptr<Process> FilterFactory(ProcessDescription desc) {
    if (desc.args.size() < 2) {
        std::cerr << "filter format is: filter [type] [value]\n";
        return (nullptr);
    }
    const std::string & name = desc.args[0];
    double value;
    if (!desc.as_double(1, value)) {
        std::cerr << desc.args[1] << " is not a valid value\n";
        return (nullptr);
    }

    FilterProcess::FilterF filt_func;
    if (name == "egate_low") {
        filt_func = FilterFunctors::FilterEnergyGateLow(value);
    } else if (name == "egate_high") {
        filt_func = FilterFunctors::FilterEnergyGateHigh(value);
    } else {
        std::cerr << "Unknown filter type: " << name << std::endl;
        return(nullptr);
    }
    return (FilterFactory(filt_func));
}

std::unique_ptr<Process> MergeFactory(MergeProcess::MergeF merge_func,
                                      TimeT value,
                                      const IdLookupT& id_map)
{
    return (std::unique_ptr<Process>(new MergeProcess(id_map, value,
                                                      merge_func)));
}

std::unique_ptr<Process> MergeFactory(
        ProcessDescription desc,
        const IdMappingT& id_maps)
{
    if (desc.args.size() < 2) {
        std::cerr << "merge format is: merge [component] [time] (options...)\n";
        return (nullptr);
    }
    const std::string & name = desc.args[0];
    TimeT value;
    if (!desc.as_time(1, value)) {
        std::cerr << desc.args[1] << " is not a valid merge time\n";
        return (nullptr);
    }

    auto map_iter = id_maps.find(name);
    if (map_iter == id_maps.end()) {
        std::cerr << "Unknown id map type: " << name << std::endl;
        return(nullptr);
    }
    const std::vector<DetIdT> id_map = (*map_iter).second;

    std::string merge_type = "max";
    if (desc.args.size() >= 3) {
        merge_type = desc.args[2];
    }

    MergeProcess::MergeF merge_func;
    if (merge_type == "max") {
        merge_func = MergeFunctors::MergeMax();
    } else if (merge_type == "first") {
        merge_func = MergeFunctors::MergeFirst();
    } else if (merge_type == "anger") {
        if (desc.args.size() != 6) {
            std::cerr << "Error: anger merge requires 3 block mapping names\n";
            return(nullptr);
        }
        for (size_t idx = 3; idx < 6; ++idx) {
            if (!id_maps.count(desc.args[idx])) {
                std::cerr << "Unknown mapping name for anger merge: "
                          << desc.args[idx] << "\n";
                return (nullptr);
            }
        }
        const auto& bx = (*id_maps.find(desc.args[3])).second;
        const auto& by = (*id_maps.find(desc.args[4])).second;
        const auto& bz = (*id_maps.find(desc.args[5])).second;

        try {
            merge_func = MergeFunctors::MergeAnger(id_map, bx, by, bz);
        } catch (const std::runtime_error e) {
            std::cerr << e.what() << std::endl;
            return (nullptr);
        }
    } else {
        std::cerr << "Unknown merge type: " << merge_type << std::endl;
        return(nullptr);
    }
    return (MergeFactory(merge_func, value, id_map));
}

std::unique_ptr<Process> SortFactory(TimeT value) {
    return (std::unique_ptr<Process>(new SortProcess(value)));
}

std::unique_ptr<Process> ProcessFactory(
        ProcessDescription desc,
        const IdMappingT& id_maps)
{
    // TODO: replace type string with enum
    if (desc.type == "merge") {
        return (MergeFactory(desc, id_maps));
    } else if (desc.type == "deadtime") {
        return (DeadtimeFactory(desc, id_maps));
    } else if (desc.type == "filter") {
        return (FilterFactory(desc));
    } else if (desc.type == "blur") {
        return (BlurFactory(desc));
    } else if (desc.type == "coinc") {
        return (CoincFactory(desc));
    } else {
        std::cerr << "Process Type not supported: " << desc.type << std::endl;
        return (nullptr);
    }
}

}
