/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

// New for Version 3.0 and later.
//  All "Color3" and "Color4" functions are just named "Color".
//  Colors are always VectorR3's.

#ifndef MATERIAL_BASE_H
#define MATERIAL_BASE_H

#include "assert.h"
#include <VrMath/LinearR3.h>
#include <string>

class VisiblePoint;
class Light;

// This is a purely abstract class intended as the base class for different
//	kinds of materials.  The materials to be supported are
//
//	  Material   (Phong lighting model).  This is the most commonly used one.
//
//	  MaterialCT (Variation of Blinn and Cook-Torrance lighting model)

class MaterialBase
{

public:
    MaterialBase() = default;
    MaterialBase(const std::string & name) : name(name) {}
    virtual ~MaterialBase() {};
    virtual bool IsReflective() const = 0;
    virtual bool IsTransmissive() const =0;
    virtual bool CalcRefractDir( const VectorR3& normal,
                                 const VectorR3& indir, VectorR3& outdir ) const = 0;
    virtual VectorR3 GetReflectionColor( const VisiblePoint& visPoint,
                                         const VectorR3& outDir,
                                         const VectorR3& fromDir) const = 0;
    virtual VectorR3 GetTransmissionColor( const VisiblePoint& visPoint,
                                           const VectorR3& outDir,
                                           const VectorR3& fromDir) const = 0;
    virtual void CalcLocalLighting(
        VectorR3& colorReturned, const Light& light,
        const VectorR3& percentLit, double lightAttenuation,
        const VectorR3& N, const VectorR3& V,
        const VectorR3& L, const VectorR3* H ) const = 0;


    void SetColorAmbient( const VectorR3& color );
    void SetColorDiffuse( const VectorR3& color );
    void SetColorAmbientDiffuse( const VectorR3& color );
    void SetColorSpecular( const VectorR3& color );
    void SetColorEmissive( const VectorR3& color );

    const VectorR3& GetColorAmbient() const;
    const VectorR3& GetColorDiffuse() const;
    const VectorR3& GetColorSpecular() const;
    const VectorR3& GetColorEmissive() const;

    virtual double GetPhongShininess() const
    {
        return 50;
    }
    std::string GetName() const {
        return(name);
    }

protected:

    VectorR3 ColorAmbient;
    VectorR3 ColorDiffuse;
    VectorR3 ColorSpecular;
    VectorR3 ColorEmissive;

public:
    static bool CalcRefractDir( double indexOfRefraction, double indexOfRefractionInv,
                                const VectorR3& normal, const VectorR3& indir,
                                VectorR3& outdir );

    virtual MaterialBase* Clone() const = 0;

    std::string name;
    void SetName(const std::string & mat_name);

};

inline void MaterialBase::SetName(const std::string & mat_name)
{
    name = mat_name;
}

inline void MaterialBase::SetColorAmbient( const VectorR3& color )
{
    ColorAmbient = color;
}

inline void MaterialBase::SetColorDiffuse( const VectorR3& color )
{
    ColorDiffuse = color;
}

inline void MaterialBase::SetColorAmbientDiffuse( const VectorR3& color )
{
    SetColorAmbient( color );
    SetColorDiffuse( color );
}

inline void MaterialBase::SetColorSpecular( const VectorR3& color )
{
    ColorSpecular = color;
}

inline void MaterialBase::SetColorEmissive( const VectorR3& color )
{
    ColorEmissive = color;
}

inline const VectorR3& MaterialBase::GetColorAmbient() const
{
    return ColorAmbient;
}

inline const VectorR3& MaterialBase::GetColorDiffuse() const
{
    return ColorDiffuse;
}

inline const VectorR3& MaterialBase::GetColorSpecular() const
{
    return ColorSpecular;
}

inline const VectorR3& MaterialBase::GetColorEmissive() const
{
    return ColorEmissive;
}

// General purpose calculation of refraction direction.
// Return false if "total internal reflection".
inline bool MaterialBase::CalcRefractDir( double indexOfRefraction, double indexOfRefractionInv,
        const VectorR3& normal, const VectorR3& indir, VectorR3& outdir )
{
    if ( indexOfRefraction==1.0 ) {
        outdir = indir;
        return true;
    }
    double ip = normal^indir;	// cosine of incident angle
    bool fromoutside = ( ip<0.0 );
    double etaInv;		// index of refraction from indir to outdir
    etaInv = fromoutside ? indexOfRefractionInv : indexOfRefraction;
    VectorR3 Tlat = normal;
    Tlat *= -ip;
    Tlat += indir;				// Lateral part of indir (parallel to surface)

    Tlat *= etaInv;				// Now lateral part of outdir

    // TsinSq = sine(transmission angle) squared
    double TsinSq = Tlat.NormSq();
    if (TsinSq >= 1.0 ) {
        return false;		// No transmission
    }
    outdir = normal;
    if ( fromoutside ) {
        outdir.Negate();
    }
    outdir *= sqrt(1.0-TsinSq);	// mult by Cosine(transmission angle)
    outdir += Tlat;
    outdir.ReNormalize();
    assert ( fabs(outdir.NormSq()-1.0)<0.000001 );
    return true;
}

#endif  // MATERIAL_BASE_H
