#ifndef GRAY_RECTSOURCE_H
#define GRAY_RECTSOURCE_H

/*
 * SimpleNurbs.h
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 */
// Function prototypes
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <vector>
#include "../Graphics/Material.h"
#include "../Graphics/TransformViewable.h"
#include "../VrMath/MathMisc.h"
#include "../VrMath/LinearR3.h"
#include "Source.h"

using namespace std;

class RectSource : public Source
{
public:
    RectSource();
    RectSource(const VectorR3 &pos, const VectorR3 &sz, double act);
    void SetSize(const VectorR3 &size);

    void virtual Decay(unsigned int photon_number);
    bool virtual Inside(const VectorR3 & pos) const;
private:
    VectorR3 size;
};

#endif /*GRAYSOURCE_H_*/
