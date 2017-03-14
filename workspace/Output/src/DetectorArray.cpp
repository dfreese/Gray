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
