/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Daq/Mapping.h>
#include <fstream>
#include <sstream>
#include <Output/IO.h>

int Mapping::LoadMapping(std::istream& input, IdMappingT& id_maps) {
    if (!input) {
        return(-1);
    }
    std::string headers;
    // Find the first non-blank line, including comments
    while (IO::GetLineCommented(input, headers)) {
        if (!headers.empty()) {
            break;
        }
    }
    std::stringstream head_ss(headers);
    std::string header;
    std::vector<std::string> header_vec;
    while (head_ss >> header) {
        header_vec.push_back(header);
        id_maps[header] = std::vector<DetIdT>();
    }

    std::string line;
    int no_detectors = 0;
    while (IO::GetLineCommented(input, line)) {
        if (line.empty()) {
            continue;
        }
        std::stringstream line_ss(line);
        for (const auto & header: header_vec) {
            DetIdT val;
            if ((line_ss >> val).fail()) {
                return(-2);
            }
            id_maps[header].push_back(val);
        }
        no_detectors++;
    }
    return(no_detectors);
}

int Mapping::LoadMapping(const std::string& filename, IdMappingT& id_maps) {
    std::ifstream input(filename);
    return (LoadMapping(input, id_maps));
}

bool Mapping::WriteMapping(std::ostream& output, const IdMappingT& mapping) {
    size_t no_detectors = 0;
    for (auto it = mapping.begin(); it != mapping.end(); ++it) {
        output << (*it).first;
        if (std::next(it) != mapping.end()) {
            output << " ";
        } else {
            no_detectors = (*mapping.begin()).second.size();
            output << "\n";
        }
    }

    for (size_t ii = 0; ii < no_detectors; ++ii) {
        for (auto it = mapping.begin(); it != mapping.end(); ++it) {
            output << (*it).second[ii];
            if (std::next(it) != mapping.end()) {
                output << " ";
            }
        }
        output << "\n";
    }
    return (output.good());
}

bool Mapping::WriteMapping(const std::string& filename,
                           const IdMappingT& mapping)
{
    std::ofstream output(filename);
    return (WriteMapping(output, mapping));
}
