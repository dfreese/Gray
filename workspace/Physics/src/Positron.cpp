#include <Physics/Positron.h>

using namespace std;

Positron::Positron()
{
    positronRange = false;
}

void Positron::SetEnergy(double e)
{
    energy = e;
};

double Positron::GetEnergy() const
{
    return energy;
}

PositronDecay * Positron::GetPositronDecay()
{
    return(&p);
}

void Positron::PositronRange(PositronDecay & p)
{
    if (positronRange) {
        if (positronRangeGaussian) {
            p.SetPositronRange(positronFWHM,positronMaxRange);
        } else if (positronRangeCusp) {
            p.SetPositronRange(positronC,positronK1,positronK2,positronMaxRange);
        }
    } else {
        p.ClearPositronRange();
    }
}

void Positron::SetPositronRange(double gauss, double max_range)
{
    positronRange = true;
    positronRangeCusp = false;
    positronRangeGaussian = true;
    positronFWHM = gauss; // expressed in mm
    positronC = -1.0;
    positronK1 = -1.0;
    positronK2 = -1.0;
    positronMaxRange = max_range;		// expressed in mm
}

void Positron::SetPositronRange(double C, double e1, double e2, double max_range)
{
    positronRange = true;
    positronRangeCusp = true;
    positronRangeGaussian = false;
    positronFWHM = -1.0;
    positronC = C; // dimensionless
    positronK1 = e1; // expressed in mm^-1
    positronK2 = e2; // expressed in mm^-1
    positronMaxRange = max_range; // expressed in mm
}
