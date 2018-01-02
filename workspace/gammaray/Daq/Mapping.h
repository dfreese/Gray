#ifndef mapping_h
#define mapping_h
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <Daq/Process.h>

namespace Mapping {
    using DetIdT = Process::DetIdT;
    int LoadMapping(
            std::istream& input,
            std::map<std::string, std::vector<DetIdT>> & id_maps);
    int LoadMapping(
            const std::string& filename,
            std::map<std::string, std::vector<DetIdT>> & id_maps);
}

#endif // mapping_h
