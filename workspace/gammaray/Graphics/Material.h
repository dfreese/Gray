/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef MATERIAL_BASE_H
#define MATERIAL_BASE_H

#include <cassert>
#include <cmath>
#include <string>
#include <VrMath/LinearR3.h>

class VisiblePoint;
class Light;

//  RayTrace had a MaterialBase class that allowed switching between Phong and
//  Cook-Torrance lighting models.  For Gray, we have just picked Phong, and
//  eliminated Cook-Torrance and MaterialBase.  This will allow for cleaner
//  inheretance down the line for Material Properties specific to PET.

class Material {
public:
    static const Material Default;

    Material() = default;
    Material(
            int id, const std::string & name, bool sensitive, bool interactive);
    virtual ~Material() = default;
    bool IsReflective() const;
    bool IsTransmissive() const;
    bool CalcRefractDir(
            const VectorR3& normal,
            const VectorR3& indir,
            VectorR3& outdir) const;
    VectorR3 GetReflectionColor(
            const VisiblePoint& visPoint,
            const VectorR3& outDir,
            const VectorR3& fromDir) const;
    VectorR3 GetTransmissionColor(
            const VisiblePoint& visPoint,
            const VectorR3& outDir,
            const VectorR3& fromDir) const;
    void CalcLocalLighting(
            VectorR3& colorReturned, const Light& light,
            const VectorR3& percentLit, double lightAttenuation,
            const VectorR3& N, const VectorR3& V,
            const VectorR3& L, const VectorR3* H ) const;

    void SetColorAmbient(const VectorR3& color);
    void SetColorDiffuse(const VectorR3& color);
    void SetColorAmbientDiffuse(const VectorR3& color);
    void SetColorSpecular(const VectorR3& color);
    void SetColorEmissive(const VectorR3& color);
    void SetColorTransmissive(const VectorR3& color);
    void SetColorReflective(const VectorR3& color);

    const VectorR3& GetColorAmbient() const;
    const VectorR3& GetColorDiffuse() const;
    const VectorR3& GetColorSpecular() const;
    const VectorR3& GetColorEmissive() const;
    const VectorR3& GetColorTransmissive() const;
    const VectorR3& GetColorReflective() const;

    void SetShininess(double exponent);
    double GetPhongShininess() const;
    double GetShininess() const;

    int GetId() const;
    std::string GetName() const;
    void SetName(const std::string & mat_name);

    void SetIndexOfRefraction(double x);
    double GetIndexOfRefraction() const;

    static bool CalcRefractDir( double indexOfRefraction, double indexOfRefractionInv,
                                const VectorR3& normal, const VectorR3& indir,
                                VectorR3& outdir );
    bool IsSensitive() const;
    bool InteractionsEnabled() const;

private:
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

    std::string name;
    int id = -1;
    bool sensitive = false;
    bool interactive = true;
};

#endif  // MATERIAL_BASE_H
