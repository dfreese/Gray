#include <Sources/AnnulusCylinderSource.h>
#include <Random/Random.h>

AnnulusCylinderSource::AnnulusCylinderSource() :
    AnnulusCylinderSource({0, 0, 0}, 1.0, {0, 0, 1}, 0)
{
}

AnnulusCylinderSource::AnnulusCylinderSource(const VectorR3 & position,
                                             double radius,
                                             VectorR3 L, double activity) :
    Source(position, activity),
    radius(radius),
    length(L.Norm()),
    local_to_global(RefAxisPlusTransToMap(L.MakeUnit(), position))

{
}

VectorR3 AnnulusCylinderSource::Decay() const {
    return(local_to_global * Random::UniformAnnulusCylinder(length, radius));
}

bool AnnulusCylinderSource::Inside(const VectorR3 & pos) const
{
    // Nothing can be inside of an Annulus which is infinitely thin.
    return (false);
}
