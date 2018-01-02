#include <Daq/Mapping.h>
#include <fstream>
#include <sstream>
#include <Output/IO.h>

int Mapping::LoadMapping(
        std::istream& input,
        std::map<std::string, std::vector<DetIdT>> & id_maps)
{
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

int Mapping::LoadMapping(
        const std::string& filename,
        std::map<std::string, std::vector<DetIdT>> & id_maps)
{
    std::ifstream input(filename);
    return (LoadMapping(input, id_maps));
}
