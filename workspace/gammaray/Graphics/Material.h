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

#include <cassert>
#include <cmath>
#include <string>
#include <Graphics/Light.h>
#include <VrMath/LinearR3.h>

class VisiblePoint;
class Light;

// This is a purely abstract class intended as the base class for different
//	kinds of materials.  The materials to be supported are
//
//	  Material   (Phong lighting model).  This is the most commonly used one.
//
//	  MaterialCT (Variation of Blinn and Cook-Torrance lighting model)

class Material
{

public:
    static const Material Default;

    Material() = default;
    Material(const std::string & name) : name(name) {}
    virtual ~Material() {};
    bool IsReflective() const {
        return ReflectiveFlag;
    }
    bool IsTransmissive() const {
        return TransmissiveFlag;
    }
    bool CalcRefractDir(
            const VectorR3& normal,
            const VectorR3& indir,
            VectorR3& outdir) const
    {
        return Material::CalcRefractDir(
                IndexOfRefraction, IndexOfRefractionInv,
                normal, indir, outdir);
    }
    VectorR3 GetReflectionColor(
            const VisiblePoint& visPoint,
            const VectorR3& outDir,
            const VectorR3& fromDir) const
    {
        return ColorReflective;
    }
    VectorR3 GetTransmissionColor(
            const VisiblePoint& visPoint,
            const VectorR3& outDir,
            const VectorR3& fromDir) const
    {
        return ColorTransmissive;
    }
    void CalcLocalLighting(
            VectorR3& colorReturned, const Light& light,
            const VectorR3& percentLit, double lightAttenuation,
            const VectorR3& N, const VectorR3& V,
            const VectorR3& L, const VectorR3* H ) const;


    void SetColorAmbient(const VectorR3& color) {
        ColorAmbient = color;
    }
    void SetColorDiffuse(const VectorR3& color) {
        ColorDiffuse = color;
    }
    void SetColorAmbientDiffuse(const VectorR3& color) {
        ColorAmbient = color;
        ColorDiffuse = color;
    }
    void SetColorSpecular(const VectorR3& color) {
        ColorSpecular = color;
    }
    void SetColorEmissive(const VectorR3& color) {
        ColorEmissive = color;
    }
    void SetColorTransmissive(const VectorR3& color) {
        TransmissiveFlag = (color.NormSq() == 0.0);
        ColorTransmissive = color;
    }
    void SetColorReflective(const VectorR3& color) {
        ReflectiveFlag = (color.NormSq() != 0.0);
        ColorReflective = color;
    }


    const VectorR3& GetColorAmbient() const {
        return ColorAmbient;
    }
    const VectorR3& GetColorDiffuse() const {
        return ColorDiffuse;
    }
    const VectorR3& GetColorSpecular() const {
        return ColorSpecular;
    }
    const VectorR3& GetColorEmissive() const {
        return ColorEmissive;
    }
    const VectorR3& GetColorTransmissive() const {
        return ColorTransmissive;
    }
    const VectorR3& GetColorReflective() const {
        return ColorReflective;
    }

    void SetShininess(double exponent) {
        Shininess = exponent;
    }
    double GetPhongShininess() const {
        return Shininess;
    }
    double GetShininess()  const {
        return Shininess;
    }

    std::string GetName() const {
        return(name);
    }

    void SetIndexOfRefraction( double x ) {
        IndexOfRefraction = x;
        IndexOfRefractionInv = 1.0/x;
    }

    double GetIndexOfRefraction() const {
        return IndexOfRefraction;
    }

protected:
    VectorR3 ColorAmbient = {0.2, 0.2, 0.2};
    VectorR3 ColorDiffuse = {0.5, 0.5, 0.5};
    VectorR3 ColorSpecular = {0.2, 0.2, 0.2};
    VectorR3 ColorEmissive = {0.0, 0.0, 0.2};
    VectorR3 ColorTransmissive = {0.0, 0.0, 0.0};
    VectorR3 ColorReflective = {0.1, 0.1, 0.1};		// Global reflection color coefficients

    double IndexOfRefraction = 1.0;
    double IndexOfRefractionInv = 1.0;	// 1/(index of refraction)

    bool TransmissiveFlag = false;		// Is transmissive color non-black?
    bool ReflectiveFlag = false;		// Is reflective color non-black?

    double Shininess = 0.0f; // Shininess exponent
    bool isError = false;

public:
    static bool CalcRefractDir( double indexOfRefraction, double indexOfRefractionInv,
                                const VectorR3& normal, const VectorR3& indir,
                                VectorR3& outdir );

    std::string name;
    void SetName(const std::string & mat_name) {
        name = mat_name;
    }

};

#endif  // MATERIAL_BASE_H
