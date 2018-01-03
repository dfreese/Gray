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
