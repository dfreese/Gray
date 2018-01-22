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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <VrMath/LinearR3.h>
#include <Graphics/MaterialBase.h>

// the Material class inplements Phong lighting.  The base class MaterialBase
//		has the basic date elements that are common to Phong lighting and
//		Cook-Torrance lighting

class Material : public MaterialBase
{

public:

    static const Material Default;

    Material()
    {
        Reset();
    }

    Material(const std::string & name) : MaterialBase(name) {
        Reset();
    }

    void Reset() {
        Shininess = 0.0f;
        SetColorAmbient({0.2, 0.2, 0.2});
        SetColorDiffuse({0.5, 0.5, 0.5});
        SetColorSpecular({0.2, 0.2, 0.2});
        SetColorEmissive({0.0, 0.0, 0.2});
        SetColorTransmissive({0.0, 0.0, 0.0});
        SetColorReflective({0.1, 0.1, 0.1});
        SetIndexOfRefraction( 1.0 );
        isError = false;
    }

    void SetShininess(double exponent) {
        Shininess = exponent;
    }

    double GetPhongShininess() const {
        return Shininess;
    }

    void SetIndexOfRefraction( double x ) {
        IndexOfRefraction = x;
        IndexOfRefractionInv = 1.0/x;
    }

    double GetIndexOfRefraction() const {
        return IndexOfRefraction;
    }

    bool CalcRefractDir(
            const VectorR3& normal,
            const VectorR3& indir,
            VectorR3& outdir) const
    {
        return MaterialBase::CalcRefractDir(
                IndexOfRefraction, IndexOfRefractionInv,
                normal, indir, outdir);
    }


    bool IsTransmissive() const {
        return TransmissiveFlag;
    }

    bool IsReflective() const
    {
        return ReflectiveFlag;
    }

    void SetColorTransmissive(const VectorR3& color) {
        TransmissiveFlag = (color.NormSq() == 0.0);
        ColorTransmissive = color;
    }

    void SetColorReflective(const VectorR3& color) {
        ReflectiveFlag = (color.NormSq() != 0.0);
        ColorReflective = color;
    }

    double GetShininess()  const
    {
        return Shininess;
    }

    const VectorR3& GetColorTransmissive() const {
        return ColorTransmissive;
    }
    const VectorR3& GetColorReflective() const {
        return ColorReflective;
    }

    virtual VectorR3 GetReflectionColor(
            const VisiblePoint& visPoint,
            const VectorR3& outDir,
            const VectorR3& fromDir) const
    {
        return GetColorReflective();
    }
    virtual VectorR3 GetTransmissionColor(
            const VisiblePoint& visPoint,
            const VectorR3& outDir,
            const VectorR3& fromDir) const
    {
        return GetColorTransmissive();
    }

    virtual void CalcLocalLighting(
        VectorR3& colorReturned, const Light& light,
        const VectorR3& percentLit, double lightAttenuation,
        const VectorR3& N, const VectorR3& V,
        const VectorR3& L, const VectorR3* H ) const;

    MaterialBase* Clone() const;


protected:
    VectorR3 ColorTransmissive;
    VectorR3 ColorReflective;		// Global reflection color coefficients

    double IndexOfRefraction;
    double IndexOfRefractionInv;	// 1/(index of refraction)

    bool TransmissiveFlag;		// Is transmissive color non-black?
    bool ReflectiveFlag;		// Is reflective color non-black?

    double Shininess;			// Shininess exponent
    bool isError;

};

#endif // MATERIAL_H
