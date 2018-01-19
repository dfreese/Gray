/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include <Output/DetectorArray.h>
#include <fstream>
#include <VrMath/LinearR3.h>
#include <VrMath/LinearR4.h>

DetectorArray::DetectorArray()
{
}

int DetectorArray::AddDetector(const VectorR3 & pos, const VectorR3 &size,
                               const RigidMapR3 & map, int x, int y, int z,
                               int bl)
{
    int detector_id = detectors.size();
    VectorR3 tpos = pos;
    map.Transform(&tpos);
    detectors.push_back(Detector(detector_id, tpos, size, map, x, y, z, bl));
    return detector_id;
}

bool DetectorArray::WritePositions(std::ostream& os) const {
    if (!os) {
        return (false);
    }
    for (const auto & d: detectors) {
        os << d << "\n";
    }
    return (os.good());
}

bool DetectorArray::WritePositions(const std::string& filename) const {
    std::ofstream output(filename);
    return (WritePositions(output));
}

Mapping::IdMappingT DetectorArray::DefaultMapping() const {
    Mapping::IdMappingT map;
    map.emplace("detector", Mapping::IdLookupT(detectors.size()));
    map.emplace("block", Mapping::IdLookupT(detectors.size()));
    map.emplace("bx", Mapping::IdLookupT(detectors.size()));
    map.emplace("by", Mapping::IdLookupT(detectors.size()));
    map.emplace("bz", Mapping::IdLookupT(detectors.size()));
    for (size_t ii = 0; ii < detectors.size(); ++ii) {
        auto & d = detectors[ii];
        map["detector"][ii] = d.detector_id;
        map["block"][ii] = d.block;
        map["bx"][ii] = d.idx[0];
        map["by"][ii] = d.idx[1];
        map["bz"][ii] = d.idx[2];
    }
    return(map);
}

Mapping::IdMappingT DetectorArray::Mapping() const {
    if (mapping_set) {
        return (mapping);
    } else {
        return (DefaultMapping());
    }
}

bool DetectorArray::LoadMapping(const std::string& filename) {
    if (Mapping::LoadMapping(filename, mapping)) {
        mapping_set = true;
        return (true);
    } else {
        return (false);
    }
}

bool DetectorArray::WriteDefaultMapping(const std::string& filename) const {
    return (Mapping::WriteMapping(filename, Mapping()));
}
