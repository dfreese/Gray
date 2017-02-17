#include <Physics/Isotope.h>

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
}
