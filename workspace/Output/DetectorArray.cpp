#include <DetectorArray.h>

DetectorArray::DetectorArray()
{
}

DetectorArray::~DetectorArray()
{
    // delete elements in the array
    while (!d.IsEmpty()) {
        Detector * detector = d.Pop();
        delete detector;
    }
}

unsigned DetectorArray::AddDetector(const VectorR3 & pos, const VectorR3 &size, const RigidMapR3 & map, const double time_res, const double eres, unsigned x, unsigned y, unsigned z, unsigned bl)
{
    unsigned detector_id = d.SizeUsed();
    VectorR3 tpos = pos;
    map.Transform(&tpos);
    Detector * nd = new Detector(detector_id, tpos, size, map, time_res, eres, x, y, z, bl);

    // Decay time is set to 10e-9, because this is the approximate time that the DAQ can distinguish events

    nd->SetDecayTime(10e-9);
    d.Push(nd);
    return detector_id;
}

void DetectorArray::OutputDetectorArray()
{
    for (int i = 0; i < d.SizeUsed(); i++) {
        cout << *(d[i]);
    }
}

ostream& operator<< ( ostream& os, const DetectorArray& d )
{
    for(int i = 0; i < d.d.SizeUsed(); i++) {
        os << *d.d.GetEntry(i);
        os << endl;
    }
    return os;
}
