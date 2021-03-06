/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef DETECTORARRAY_H_
#define DETECTORARRAY_H_
#include <map>
#include <vector>
#include <ostream>
#include "Gray/Daq/Mapping.h"
#include "Gray/Output/Detector.h"

class VectorR3;
class RigidMapR3;

class DetectorArray
{
public:
    DetectorArray();
    // returns detector_id
    int AddDetector(const VectorR3 & pos, const VectorR3 &size,
                    const RigidMapR3 & map, int x, int y, int z, int bl);
    bool WritePositions(std::ostream& os) const;
    bool WritePositions(const std::string& filename) const;
    Mapping::IdMappingT Mapping() const;
    bool LoadMapping(const std::string& filename);
    bool WriteDefaultMapping(const std::string& filename) const;

private:
    std::vector<Detector> detectors;
    Mapping::IdMappingT DefaultMapping() const;
    bool mapping_set = false;
    Mapping::IdMappingT mapping;
};

#endif /*DETECTORARRAY_H_*/
