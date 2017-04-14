#include <Output/DetectorArray.h>
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

void DetectorArray::OutputDetectorArray()
{
    for (int i = 0; i < detectors.size(); i++) {
        cout << (detectors[i]);
    }
}

ostream& operator<< ( ostream& os, const DetectorArray& d )
{
    for (const auto & detector: d.detectors) {
        os << detector << endl;
    }
    return os;
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
