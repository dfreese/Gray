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

std::map<std::string, std::vector<int>> DetectorArray::default_mapping() {
    std::map<std::string, std::vector<int>> map;
    map["detector"] = std::vector<int>();
    map["block"] = std::vector<int>();
    map["bx"] = std::vector<int>();
    map["by"] = std::vector<int>();
    map["bz"] = std::vector<int>();
    for (const auto & d: detectors){
        map["detector"].push_back(d.detector_id);
        map["block"].push_back(d.block);
        map["bx"].push_back(d.idx[0]);
        map["by"].push_back(d.idx[1]);
        map["bz"].push_back(d.idx[2]);
    }
    return(map);
}

void DetectorArray::OutputDetectorArray()
{
    for (int i = 0; i < detectors.size(); i++) {
        cout << (detectors[i]);
    }
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

void DetectorArray::WriteBasicMap(std::ostream & os,
                                  const std::string & detector_name,
                                  const std::string & block_name,
                                  const std::string & bx_name,
                                  const std::string & by_name,
                                  const std::string & bz_name)
{
    os << detector_name << " " << block_name << " " << bx_name  << " "
       << by_name << " " << bz_name << "\n";
    for (const auto & d: detectors){
        os << d.detector_id << " " << d.block << " " << d.idx[0]  << " "
           << d.idx[1] << " " << d.idx[2] << "\n";
    }
}


