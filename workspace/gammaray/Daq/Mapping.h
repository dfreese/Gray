/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef mapping_h
#define mapping_h
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <Daq/Process.h>

namespace Mapping {
    using DetIdT = Process::DetIdT;
    using IdLookupT = std::vector<DetIdT>;
    using IdMappingT = std::map<std::string, IdLookupT>;
    int LoadMapping(std::istream& input, IdMappingT& id_maps);
    int LoadMapping(const std::string& filename, IdMappingT& id_maps);
    bool WriteMapping(std::ostream& output, const IdMappingT& mapping);
    bool WriteMapping(const std::string& filename, const IdMappingT& mapping);
}

#endif // mapping_h
