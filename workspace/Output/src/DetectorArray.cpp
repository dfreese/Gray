#include <Output/DetectorArray.h>
#include <VrMath/LinearR3.h>
#include <VrMath/LinearR4.h>

DetectorArray::DetectorArray()
{
}

DetectorArray::~DetectorArray()
{
    // delete elements in the array
    while (!d.empty()) {
        delete d.back();
        d.pop_back();
    }
}

unsigned DetectorArray::AddDetector(const VectorR3 & pos, const VectorR3 &size,
                                    const RigidMapR3 & map, const unsigned x,
                                    unsigned y, unsigned z, unsigned bl)
{
    unsigned detector_id = d.size();
    VectorR3 tpos = pos;
    map.Transform(&tpos);
    Detector * nd = new Detector(detector_id, tpos, size, map, x, y, z, bl);

    // Decay time is set to 10e-9, because this is the approximate time that the DAQ can distinguish events

    nd->SetDecayTime(10e-9);
    d.push_back(nd);
    return detector_id;
}

void DetectorArray::OutputDetectorArray()
{
    for (int i = 0; i < d.size(); i++) {
        cout << *(d[i]);
    }
}

ostream& operator<< ( ostream& os, const DetectorArray& d )
{
    for(int i = 0; i < d.d.size(); i++) {
        os << *d.d.at(i);
        os << endl;
    }
    return os;
}
