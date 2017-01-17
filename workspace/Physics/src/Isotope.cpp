#include <Physics/Isotope.h>
#include <Physics/F18.h>
#include <Physics/IN110.h>
#include <Physics/ZR89.h>
#include <Physics/BackBack.h>
#include <Physics/Beam.h>

Isotope::Isotope()
{

    source_num = -1;

    EMPTY.pos.SetZero();
    EMPTY.dir.SetUnitZ();
    EMPTY.id = -1;
    EMPTY.energy = 0.0;
    EMPTY.time = 0.0;
    EMPTY.det_id = -1;
    EMPTY.phantom_scatter = false;

    mapIsotope["F18"] = ISO_F18;
    mapIsotope["IN110"] = ISO_IN110;
    mapIsotope["ZR89"] = ISO_ZR89;
    mapIsotope["BackBack"] = ISO_BACKBACK;
    mapIsotope["Beam"] = ISO_BEAM;
}

ISOTOPE_TYPE Isotope::getType(const char * iso)
{
    switch (mapIsotope[iso]) {
    case ISO_F18:
    case ISO_IN110:
    case ISO_ZR89:
    case ISO_BACKBACK:
    case ISO_BEAM:
        return mapIsotope[iso];
        break;
    default:
        cout << "ERROR: Isotope: " << iso << " doesn't exist.\n";
        return ISO_BACKBACK;
    }
}

Isotope * Isotope::newIsotope(ISOTOPE_TYPE type)
{
    switch(type) {
    case ISO_F18:
        return(new F18());
        break;
    case ISO_IN110:
        return(new IN110());
        break;
    case ISO_BACKBACK:
        return(new BackBack());
        break;
    case ISO_BEAM:
        return(new Beam());
        break;
    case ISO_ZR89:
        return(new ZR89());
        break;
    default:
        cerr << "Unkown Isotope:" << type << endl;
        return (new BackBack());
        break;
    }
}
