#ifndef BEAMPOINTSOURCE_H_
#define BEAMPOINTSOURCE_H_

#include <memory>
#include <Sources/Source.h>

class BeamPointSource : public Source
{
public:
    BeamPointSource(const VectorR3 &p, const VectorR3 &a, double angle, double act);
    // gets a beam positron instead of a regular one
    VectorR3 Decay() override;
    bool Inside(const VectorR3 & pos) const override;
    void SetIsotope(std::unique_ptr<Isotope> i) override;
private:
    VectorR3 beam_axis;
    double beam_angle;
};

#endif /*BEAMPOINTSOURCE_H_*/
