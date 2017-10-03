#include <Sources/AnnulusCylinderSource.h>
#include <Random/Random.h>

AnnulusCylinderSource::AnnulusCylinderSource() :
    Source(),
    radius(1.0),
    length(1.0),
    axis(0.0, 0.0, 1.0)
{
}

AnnulusCylinderSource::AnnulusCylinderSource(const VectorR3 &p, double rad, VectorR3 L, double act) :
    Source(p, act)
{
    SetRadius(rad);
    SetAxis(L.MakeUnit());
}

VectorR3 AnnulusCylinderSource::Decay(int photon_number, double time)
{
    VectorR3 roted = local_to_global * Random::UniformAnnulusCylinder(length, radius);
    isotope->Decay(photon_number, time, source_num, roted);
    return(roted);
}

void AnnulusCylinderSource::SetRadius(double r)
{
    radius = r;
}

void AnnulusCylinderSource::SetAxis(VectorR3 L)
{
    length = L.Norm();
    axis = L.MakeUnit();
    local_to_global = RefAxisPlusTransToMap(axis, position);
}

bool AnnulusCylinderSource::Inside(const VectorR3 & pos) const
{
    // Nothing can be inside of an Annulus which is infinitely thin.
    return (false);
}
